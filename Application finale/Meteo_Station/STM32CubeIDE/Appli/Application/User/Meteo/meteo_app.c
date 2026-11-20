/*
 * meteo_app.c
 * Station meteo : capteurs X-NUCLEO-IKS4A1 -> pretraitement -> reseau de neurones (NPU)
 *                 -> envoi sur ThingSpeak (mesures + classe predite).
 *
 * Un thread ThreadX, lance par METEO_App_Start() quand l'adresse IP est obtenue (DHCP).
 *
 * Champs ThingSpeak :
 *   field1 = Temperature (degC)
 *   field2 = Humidite (%)
 *   field3 = Barometrie : pression mesuree sur place (hPa)
 *   field4 = Pression atmospherique ramenee au niveau de la mer (hPa)
 *   field5 = Prediction : code meteo = dizaine (temps) + unite (information)
 *   field6 = Confiance de la prediction (%)
 *   field7 = Temperature predite +3h : non envoyee (le modele ne la calcule pas)
 *
 * Code meteo (field5) :
 *   dizaine : 10 Soleil, 20 Soleil et nuages, 30 Soleil et beaucoup de nuages,
 *             40 Pluie eparse, 50 Pluie, 60 Forte pluie, 70 Orage, 80 Neige, 90 Brouillard
 *   unite   : 0 rien, 1 Rafales, 2 Verglas, 3 Forte chaleur, 4/5/6 Vigilance jaune/orange/rouge
 *   Le modele actuel (4 classes) ne produit que 10, 30, 50 et 90.
 *   L'unite est calculee par des regles sur les mesures : 2 (verglas) et 3 (forte chaleur).
 *
 * Entree du modele (voir meteo_norm.h) : 12 heures x 6 grandeurs
 *   T, HR, pression ramenee au niveau de la mer, T - Td (point de rosee), sin(h), cos(h)
 *   avec h = heure UTC. La case 11 est l'heure en cours (moyenne des mesures de l'heure).
 *   Au demarrage, les 12 cases sont remplies avec la premiere mesure : le resultat
 *   n'est representatif qu'apres 12 heures de fonctionnement.
 *   L'heure UTC est lue dans l'en-tete "Date:" des reponses HTTP de ThingSpeak.
 */
#include "main.h"
#include "tx_api.h"
#include "nx_api.h"
#include "nxd_dns.h"
#include "stai.h"
#include "stai_network.h"
#include "user_init.h"
#include "sensor_task.h"
#include "thingspeak.h"
#include "meteo_norm.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

/* ---------------- Reglages ---------------- */
#define TS_WRITE_KEY        "E3QHXZ4C2TUDIC2L"  /* cle d'ecriture du canal ThingSpeak */
#define TS_FIELD_T          1
#define TS_FIELD_HR         2
#define TS_FIELD_P          3                   /* pression mesuree (barometrie) */
#define TS_FIELD_PMER       4                   /* pression niveau mer */
#define TS_FIELD_CODE       5                   /* code meteo dizaine + unite */
#define TS_FIELD_CONFIANCE  6                   /* confiance en % */
#define SEUIL_CHALEUR_C     30.0f               /* unite 3 : forte chaleur */
#define SEUIL_VERGLAS_C     1.0f                /* unite 2 : verglas possible */
#define PERIODE_S           60u                 /* 1 mesure + 1 envoi par minute (ThingSpeak : 15 s mini) */
#define ALTITUDE_M          240.0f              /* altitude du site, pour la pression niveau mer */
#define TIMEOUT_MS          10000u
#define TCP_WINDOW          512u

#define METEO_STACK_SIZE    (8u * 1024u)
#define METEO_PRIORITY      10u

/* Objets NetX Duo crees par CubeMX dans app_netxduo.c */
extern NX_IP          NetXDuoEthIpInstance;
extern NX_PACKET_POOL NxAppPool;

static TX_THREAD meteo_thread;
static ULONG     meteo_stack[METEO_STACK_SIZE / sizeof(ULONG)];
static NX_DNS    dns_client;
static ULONG     ts_ip;

/* Affichage d'un float sans printf("%f") (newlib-nano) : 1 decimale */
static const char *f1(char *b, size_t n, float v)
{
    int32_t x = (int32_t)lroundf(v * 10.0f);
    const char *s = (x < 0) ? "-" : "";
    if (x < 0) x = -x;
    snprintf(b, n, "%s%ld.%ld", s, (long)(x / 10), (long)(x % 10));
    return b;
}

/* ======================= Horloge UTC ======================= */
static int32_t utc_ref_s = -1;      /* secondes depuis minuit UTC a l'instant tick_ref */
static ULONG   tick_ref;

static int chiffre(uint8_t c) { return (c >= '0' && c <= '9') ? (c - '0') : -100; }

/* Cherche "hh:mm:ss GMT" dans la reponse, ex. "Date: Fri, 20 Nov 2026 13:45:12 GMT" */
static void horloge_depuis_http(const uint8_t *rx, size_t n)
{
    for (size_t i = 9; i + 2 < n; i++)
    {
        if (rx[i] == 'G' && rx[i + 1] == 'M' && rx[i + 2] == 'T' &&
            rx[i - 1] == ' ' && rx[i - 4] == ':' && rx[i - 7] == ':')
        {
            int hh = chiffre(rx[i - 9]) * 10 + chiffre(rx[i - 8]);
            int mm = chiffre(rx[i - 6]) * 10 + chiffre(rx[i - 5]);
            int ss = chiffre(rx[i - 3]) * 10 + chiffre(rx[i - 2]);
            if (hh >= 0 && hh < 24 && mm >= 0 && mm < 60 && ss >= 0 && ss < 61)
            {
                utc_ref_s = hh * 3600 + mm * 60 + ss;
                tick_ref  = tx_time_get();
            }
            return;
        }
    }
}

/* Heure UTC courante (0..23), ou -1 si l'heure n'est pas encore connue */
static int heure_utc(void)
{
    if (utc_ref_s < 0) return -1;
    ULONG dt = (tx_time_get() - tick_ref) / TX_TIMER_TICKS_PER_SECOND;
    return (int)((((uint32_t)utc_ref_s + dt) % 86400u) / 3600u);
}

/* ======================= Transport TCP (port 80) ======================= */
static int ts_transport(const uint8_t *tx, size_t tx_len, uint8_t *rx,
                        size_t rx_max, uint32_t timeout_ms, void *ctx)
{
    NX_TCP_SOCKET sock;
    NX_PACKET *pkt;
    ULONG tmo = (timeout_ms * NX_IP_PERIODIC_RATE) / 1000;
    size_t total = 0;
    int result = -1;
    (void)ctx;

    if (nx_tcp_socket_create(&NetXDuoEthIpInstance, &sock, "TS socket", NX_IP_NORMAL,
                             NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, TCP_WINDOW,
                             NX_NULL, NX_NULL) != NX_SUCCESS) return -1;

    if (nx_tcp_client_socket_bind(&sock, NX_ANY_PORT, tmo) != NX_SUCCESS) goto del;
    if (nx_tcp_client_socket_connect(&sock, ts_ip, 80, tmo) != NX_SUCCESS) goto unbind;

    if (nx_packet_allocate(&NxAppPool, &pkt, NX_TCP_PACKET, tmo) != NX_SUCCESS) goto disc;
    if (nx_packet_data_append(pkt, (VOID *)tx, tx_len, &NxAppPool, tmo) != NX_SUCCESS)
    { nx_packet_release(pkt); goto disc; }
    if (nx_tcp_socket_send(&sock, pkt, tmo) != NX_SUCCESS)
    { nx_packet_release(pkt); goto disc; }        /* en cas de succes, NetX libere le paquet */

    /* lecture jusqu'a la fermeture par le serveur (Connection: close) ou buffer plein */
    while (total < rx_max && nx_tcp_socket_receive(&sock, &pkt, tmo) == NX_SUCCESS)
    {
        ULONG n = 0;
        nx_packet_data_extract_offset(pkt, 0, rx + total, rx_max - total, &n);
        nx_packet_release(pkt);
        total += n;
    }
    result = (int)total;
    horloge_depuis_http(rx, total);                /* mise a l'heure a chaque reponse */

disc:   nx_tcp_socket_disconnect(&sock, tmo);
unbind: nx_tcp_client_socket_unbind(&sock);
del:    nx_tcp_socket_delete(&sock);
    return result;
}

/* Premiere mise a l'heure : simple requete HEAD, la reponse contient "Date:" */
static void demander_heure(void)
{
    static const char req[] = "HEAD / HTTP/1.1\r\nHost: " TS_HOST "\r\nConnection: close\r\n\r\n";
    static uint8_t rx[512];
    ts_transport((const uint8_t *)req, sizeof(req) - 1, rx, sizeof(rx), TIMEOUT_MS, NULL);
}

/* ======================= Fenetre de 12 heures ======================= */
enum { F_T, F_HR, F_PMER, F_ECART, F_MES };      /* 4 grandeurs mesurees par heure */

static float    fenetre[METEO_W][F_MES];          /* moyennes horaires, [METEO_W-1] = heure en cours */
static float    somme[F_MES];
static uint32_t nb_mesures;
static int      heure_cour = -1;

static void fenetre_ajouter(const float m[F_MES], int h)
{
    if (heure_cour < 0)                           /* 1re mesure : toute la fenetre */
    {
        for (int t = 0; t < METEO_W; t++) memcpy(fenetre[t], m, sizeof(fenetre[t]));
        heure_cour = h;
    }
    else if (h != heure_cour)                     /* nouvelle heure : decalage d'une case par heure */
    {
        int d = (h - heure_cour + 24) % 24;
        if (d > METEO_W) d = METEO_W;
        for (int k = 0; k < d; k++)
            memmove(fenetre[0], fenetre[1], sizeof(fenetre[0]) * (METEO_W - 1));
        heure_cour = h;
        nb_mesures = 0;
        memset(somme, 0, sizeof(somme));
    }

    nb_mesures++;
    for (int i = 0; i < F_MES; i++)
    {
        somme[i] += m[i];
        fenetre[METEO_W - 1][i] = somme[i] / (float)nb_mesures;
    }
}

/* T (degC), HR (%), P (hPa, pression locale) -> grandeurs du modele */
static void calculer_grandeurs(float T, float HR, float P, float m[F_MES])
{
    float hr = (HR < 1.0f) ? 1.0f : ((HR > 100.0f) ? 100.0f : HR);
    float g  = logf(hr / 100.0f) + 17.62f * T / (243.12f + T);   /* formule de Magnus */
    float td = 243.12f * g / (17.62f - g);                         /* point de rosee */
    float k  = 0.0065f * ALTITUDE_M;
    float p0 = P * powf(1.0f - k / (T + k + 273.15f), -5.257f);    /* pression niveau mer */

    m[F_T]     = T;
    m[F_HR]    = HR;
    m[F_PMER]  = p0;
    m[F_ECART] = T - td;
}

/* ======================= Reseau de neurones (NPU) ======================= */
STAI_NETWORK_CONTEXT_DECLARE(meteo_net, STAI_NETWORK_CONTEXT_SIZE)

static const float   in_scale[]  = STAI_NETWORK_IN_1_SCALES;
static const int32_t in_zp[]     = STAI_NETWORK_IN_1_OFFSETS;
static const float   out_scale[] = STAI_NETWORK_OUT_1_SCALES;
static const int32_t out_zp[]    = STAI_NETWORK_OUT_1_OFFSETS;
static int8_t       *ia_in, *ia_out;

/* Classe du modele -> dizaine du code meteo.
 * Ordre des classes du modele (meteo_norm.h) : Clair, Nuageux, Brouillard, Pluie */
static const uint8_t CODE_DIZAINE[METEO_NCLS] = { 10, 30, 90, 50 };
static int           ia_ok;

#if (METEO_W * METEO_C) != STAI_NETWORK_IN_1_SIZE
#error "meteo_norm.h ne correspond pas au modele genere"
#endif
#if METEO_NCLS != 4
#error "CODE_DIZAINE est prevu pour 4 classes : a adapter si le modele change"
#endif
#if METEO_NCLS != STAI_NETWORK_OUT_1_SIZE
#error "Nombre de classes different entre meteo_norm.h et le modele"
#endif

static int ia_init(void)
{
    stai_ptr ins[STAI_NETWORK_IN_NUM];
    stai_ptr outs[STAI_NETWORK_OUT_NUM];
    stai_size n;

    if (stai_runtime_init() != STAI_SUCCESS) return -1;
    if (user_stai_network_init(meteo_net) != STAI_SUCCESS) return -1;
    if (stai_network_get_inputs(meteo_net, ins, &n) != STAI_SUCCESS) return -1;
    if (stai_network_get_outputs(meteo_net, outs, &n) != STAI_SUCCESS) return -1;
    ia_in  = (int8_t *)ins[0];
    ia_out = (int8_t *)outs[0];
    return 0;
}

static int8_t quantifier(float x)
{
    float q = roundf(x / in_scale[0]) + (float)in_zp[0];
    if (q > 127.0f)  q = 127.0f;
    if (q < -128.0f) q = -128.0f;
    return (int8_t)q;
}

/* Remplit l'entree int8 du modele : normalisation (mu, sigma) puis quantification */
static void construire_entree(int8_t *in)
{
    const float deux_pi = 6.2831853f;
    for (int t = 0; t < METEO_W; t++)
    {
        int h = (heure_cour - (METEO_W - 1 - t) + 48) % 24;    /* heure UTC de la case t */
        float f[METEO_C] = {
            fenetre[t][F_T], fenetre[t][F_HR], fenetre[t][F_PMER], fenetre[t][F_ECART],
            sinf(deux_pi * (float)h / 24.0f), cosf(deux_pi * (float)h / 24.0f)
        };
        for (int c = 0; c < METEO_C; c++)
        {
            float x = (f[c] - METEO_MU[c]) / METEO_SIGMA[c];
#if METEO_INPUT_NCHW
            in[c * METEO_W + t] = quantifier(x);
#else
            in[t * METEO_C + c] = quantifier(x);
#endif
        }
    }
}

/* Lance une inference, retourne la classe (0..METEO_NCLS-1) ou -1.
 * *confiance = probabilite de la classe (softmax des sorties), en % */
static int ia_classer(float *confiance)
{
    construire_entree(ia_in);

    /* le NPU lit la RAM : vider le cache CPU de l'entree avant l'execution */
    SCB_CleanDCache_by_Addr((uint32_t *)ia_in,
                            (int32_t)((STAI_NETWORK_IN_1_SIZE_BYTES + 31u) & ~31u));

    if (stai_network_run(meteo_net, STAI_MODE_SYNC) != STAI_SUCCESS) return -1;
    /* le code genere invalide deja le cache de la sortie */

    /* sorties int8 -> scores reels, puis softmax pour la confiance */
    float z[METEO_NCLS], somme_exp = 0.0f;
    int best = 0;
    for (int k = 0; k < METEO_NCLS; k++)
    {
        z[k] = ((float)ia_out[k] - (float)out_zp[0]) * out_scale[0];
        if (z[k] > z[best]) best = k;
    }
    for (int k = 0; k < METEO_NCLS; k++) somme_exp += expf(z[k] - z[best]);
    *confiance = 100.0f / somme_exp;
    return best;
}

/* Chiffre des unites du code meteo, calcule par des regles sur les mesures.
 * 1 (rafales) et 4/5/6 (vigilances) ne peuvent pas etre deduits des capteurs : jamais envoyes. */
static int code_unite(int classe, float T, float ecart_rosee)
{
    if (T >= SEUIL_CHALEUR_C) return 3;                              /* forte chaleur */
    if (T <= SEUIL_VERGLAS_C &&
        (classe == 3 || classe == 2 || ecart_rosee < 2.0f)) return 2; /* verglas */
    return 0;
}

/* ======================= Thread principal ======================= */
static VOID meteo_thread_entry(ULONG arg)
{
    char b1[16], b2[16], b3[16];
    (void)arg;

    printf("[METEO] Demarrage\r\n");
    SENSORS_Init();

    ia_ok = (ia_init() == 0);
    printf("[IA] Initialisation %s\r\n", ia_ok ? "OK" : "ECHEC");

    /* Resolution DNS de api.thingspeak.com */
    nx_dns_create(&dns_client, &NetXDuoEthIpInstance, (UCHAR *)"DNS Client");
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
    nx_dns_packet_pool_set(&dns_client, &NxAppPool);
#endif
    nx_dns_server_add(&dns_client, IP_ADDRESS(8, 8, 8, 8));
    while (nx_dns_host_by_name_get(&dns_client, (UCHAR *)TS_HOST, &ts_ip,
                                   5 * NX_IP_PERIODIC_RATE) != NX_SUCCESS)
    {
        printf("[NET] DNS en echec, nouvel essai\r\n");
        tx_thread_sleep(5 * TX_TIMER_TICKS_PER_SECOND);
    }
    printf("[NET] %s trouve\r\n", TS_HOST);

    /* Heure UTC necessaire pour sin(h) / cos(h) */
    while (heure_utc() < 0)
    {
        demander_heure();
        if (heure_utc() < 0) tx_thread_sleep(5 * TX_TIMER_TICKS_PER_SECOND);
    }
    printf("[NET] Heure UTC : %d h\r\n", heure_utc());

    while (1)
    {
        /* 1. Mesures */
        SENSORS_dataGet();
        float T  = SENSORS_getTemperature();
        float HR = SENSORS_getHumidity();
        float P  = SENSORS_getPressure();
        printf("[CAPTEURS] T=%s C  HR=%s %%  P=%s hPa\r\n",
               f1(b1, sizeof b1, T), f1(b2, sizeof b2, HR), f1(b3, sizeof b3, P));

        /* 2. Pretraitement et fenetre de 12 heures */
        float m[F_MES];
        calculer_grandeurs(T, HR, P, m);
        fenetre_ajouter(m, heure_utc());

        /* 3. Inference sur le NPU */
        float confiance = 0.0f;
        int classe = ia_ok ? ia_classer(&confiance) : -1;
        int code = -1;
        if (classe >= 0)
        {
            code = CODE_DIZAINE[classe] + code_unite(classe, T, m[F_ECART]);
            printf("[IA] Classe : %s  code=%d  confiance=%s %%\r\n",
                   METEO_CLASSES[classe], code, f1(b1, sizeof b1, confiance));
        }
        else
            printf("[IA] Inference impossible\r\n");

        /* 4. Envoi ThingSpeak */
        ts_sample_t s;
        ts_sample_init(&s, 0);
        ts_sample_set(&s, TS_FIELD_T, T);
        ts_sample_set(&s, TS_FIELD_HR, HR);
        ts_sample_set(&s, TS_FIELD_P, P);
        ts_sample_set(&s, TS_FIELD_PMER, m[F_PMER]);
        if (code >= 0)
        {
            ts_sample_set(&s, TS_FIELD_CODE, (float)code);
            ts_sample_set(&s, TS_FIELD_CONFIANCE, confiance);
        }

        ts_status_t st = ts_send_update(ts_transport, NULL, TS_WRITE_KEY, &s);
        printf("[NET] ThingSpeak : %s (%d)\r\n", (st == TS_OK) ? "OK" : "erreur", (int)st);

        tx_thread_sleep(PERIODE_S * TX_TIMER_TICKS_PER_SECOND);
    }
}

/* A appeler une fois l'adresse IP obtenue (app_netxduo.c) */
void METEO_App_Start(void)
{
    if (tx_thread_create(&meteo_thread, "Meteo", meteo_thread_entry, 0,
                         meteo_stack, sizeof(meteo_stack),
                         METEO_PRIORITY, METEO_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    {
        printf("[METEO] Erreur creation du thread\r\n");
    }
}
