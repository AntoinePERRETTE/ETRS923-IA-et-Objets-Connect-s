/*
 * thingspeak.h - Construction des buffers HTTP pour ThingSpeak (STM32 / C99)
 * Indépendant du transport réseau (Wi-Fi AT, Ethernet/lwIP, modem GSM...).
 */
#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdint.h>
#include <stddef.h>

#define TS_HOST          "api.thingspeak.com"
#define TS_FIELDS        8      /* ThingSpeak : field1..field8                      */
#define TS_BULK_MAX      10     /* nb max de mesures par envoi groupé               */
#define TS_HDR_RESERVE   256    /* place réservée aux en-têtes HTTP (POST)          */
#define TS_BUF_SINGLE    512
#define TS_BUF_BULK      2048

/* Une mesure : valeurs + masque des champs renseignés (bit0 = field1) */
typedef struct {
    float    v[TS_FIELDS];
    uint8_t  mask;
    uint32_t dt_s;      /* secondes écoulées depuis la mesure précédente (envoi groupé) */
} ts_sample_t;

typedef struct {
    ts_sample_t s[TS_BULK_MAX];
    uint8_t     n;
} ts_bulk_t;

/*
 * Fonction de transport à fournir : envoie tx[0..tx_len) sur une connexion TCP
 * vers TS_HOST (port 80, ou 443 si votre module gère TLS), lit la réponse dans rx
 * et retourne le nb d'octets reçus (>=0) ou <0 en cas d'erreur.
 */
typedef int (*ts_transport_fn)(const uint8_t *tx, size_t tx_len,
                               uint8_t *rx, size_t rx_max,
                               uint32_t timeout_ms, void *ctx);

typedef enum {
    TS_OK            =  0,
    TS_ERR_BUF       = -1,  /* buffer trop petit                          */
    TS_ERR_NET       = -2,  /* échec du transport                         */
    TS_ERR_HTTP      = -3,  /* code HTTP inattendu                        */
    TS_ERR_REJECTED  = -4   /* ThingSpeak a répondu "0" (intervalle, clé) */
} ts_status_t;

/* --- Mesures --- */
void ts_sample_init(ts_sample_t *s, uint32_t dt_s);
void ts_sample_set(ts_sample_t *s, uint8_t field /*1..8*/, float value);

/* --- Envoi groupé --- */
void ts_bulk_clear(ts_bulk_t *b);
int  ts_bulk_add(ts_bulk_t *b, const ts_sample_t *s);   /* 0 ok, -1 plein */

/* --- Construction des buffers (retournent la longueur, ou <0) --- */
int ts_build_update_get(char *buf, size_t size, const char *write_key,
                        const ts_sample_t *s);
int ts_build_bulk_post(char *buf, size_t size, const char *channel_id,
                       const char *write_key, const ts_bulk_t *b,
                       const char **start);

/* --- Envoi complet --- */
ts_status_t ts_send_update(ts_transport_fn tp, void *ctx,
                           const char *write_key, const ts_sample_t *s);
ts_status_t ts_send_bulk(ts_transport_fn tp, void *ctx, const char *channel_id,
                         const char *write_key, const ts_bulk_t *b);

#endif /* THINGSPEAK_H */
