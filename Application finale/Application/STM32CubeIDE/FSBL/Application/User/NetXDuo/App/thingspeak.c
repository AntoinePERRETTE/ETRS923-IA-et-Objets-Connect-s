/*
 * thingspeak.c
 * Pas de printf("%f") : par défaut newlib-nano (STM32CubeIDE) ne le supporte pas.
 * Les flottants sont formatés à la main (2 décimales).
 */
#include "thingspeak.h"
#include <string.h>
#include <stdio.h>

/* ---------- écriture sécurisée dans un buffer ---------- */
typedef struct { char *b; size_t size, len; int err; } wr_t;

static void wr_init(wr_t *w, char *b, size_t size) {
    w->b = b; w->size = size; w->len = 0; w->err = 0;
    if (size) b[0] = 0; else w->err = 1;
}
static void wr_str(wr_t *w, const char *s) {
    while (*s) {
        if (w->err || w->len + 1 >= w->size) { w->err = 1; return; }
        w->b[w->len++] = *s++;
    }
    w->b[w->len] = 0;
}
static void wr_uint(wr_t *w, uint32_t v) {
    char t[11]; int i = 0;
    do { t[i++] = (char)('0' + v % 10); v /= 10; } while (v);
    char o[11]; int k = 0;
    while (i) o[k++] = t[--i];
    o[k] = 0;
    wr_str(w, o);
}
static void wr_float(wr_t *w, float v, uint8_t dec) {
    if (v != v) { wr_str(w, "0"); return; }          /* NaN -> 0 */
    if (v < 0)  { wr_str(w, "-"); v = -v; }
    uint32_t scale = 1;
    for (uint8_t i = 0; i < dec; i++) scale *= 10;
    uint32_t ip = (uint32_t)v;
    uint32_t fp = (uint32_t)((v - (float)ip) * (float)scale + 0.5f);
    if (fp >= scale) { ip++; fp -= scale; }
    wr_uint(w, ip);
    if (dec) {
        wr_str(w, ".");
        for (uint32_t d = scale / 10; d > 1 && fp < d; d /= 10) wr_str(w, "0");
        wr_uint(w, fp);
    }
}

/* ---------- mesures ---------- */
void ts_sample_init(ts_sample_t *s, uint32_t dt_s) {
    memset(s, 0, sizeof(*s));
    s->dt_s = dt_s;
}
void ts_sample_set(ts_sample_t *s, uint8_t field, float value) {
    if (field < 1 || field > TS_FIELDS) return;
    s->v[field - 1] = value;
    s->mask |= (uint8_t)(1u << (field - 1));
}
void ts_bulk_clear(ts_bulk_t *b) { b->n = 0; }
int  ts_bulk_add(ts_bulk_t *b, const ts_sample_t *s) {
    if (b->n >= TS_BULK_MAX) return -1;
    b->s[b->n++] = *s;
    return 0;
}

/* ---------- Format 1 : GET /update?api_key=...&field1=... ---------- */
int ts_build_update_get(char *buf, size_t size, const char *write_key,
                        const ts_sample_t *s) {
    wr_t w; wr_init(&w, buf, size);
    wr_str(&w, "GET /update?api_key=");
    wr_str(&w, write_key);
    for (uint8_t i = 0; i < TS_FIELDS; i++) {
        if (!(s->mask & (1u << i))) continue;
        wr_str(&w, "&field"); wr_uint(&w, i + 1u); wr_str(&w, "=");
        wr_float(&w, s->v[i], 2);
    }
    wr_str(&w, " HTTP/1.1\r\nHost: " TS_HOST "\r\nConnection: close\r\n\r\n");
    return w.err ? TS_ERR_BUF : (int)w.len;
}

/* ---------- Format 2 : POST /channels/<ID>/bulk_update.json ----------
 * Corps JSON :
 * {"write_api_key":"KEY","time_format":"relative","updates":[
 *    {"delta_t":0,"field1":23.50,"field2":61.00}, {...} ]}
 * Le corps est écrit en premier (à buf+TS_HDR_RESERVE) pour connaître
 * Content-Length, puis les en-têtes sont collés juste devant : aucun memmove
 * du corps. *start pointe sur le début de la requête complète.
 */
int ts_build_bulk_post(char *buf, size_t size, const char *channel_id,
                       const char *write_key, const ts_bulk_t *b,
                       const char **start) {
    if (size <= TS_HDR_RESERVE + 64 || b->n == 0) return TS_ERR_BUF;
    char *body = buf + TS_HDR_RESERVE;
    wr_t w; wr_init(&w, body, size - TS_HDR_RESERVE);
    wr_str(&w, "{\"write_api_key\":\""); wr_str(&w, write_key);
    wr_str(&w, "\",\"time_format\":\"relative\",\"updates\":[");
    for (uint8_t k = 0; k < b->n; k++) {
        const ts_sample_t *s = &b->s[k];
        if (k) wr_str(&w, ",");
        wr_str(&w, "{\"delta_t\":"); wr_uint(&w, s->dt_s);
        for (uint8_t i = 0; i < TS_FIELDS; i++) {
            if (!(s->mask & (1u << i))) continue;
            wr_str(&w, ",\"field"); wr_uint(&w, i + 1u); wr_str(&w, "\":");
            wr_float(&w, s->v[i], 2);
        }
        wr_str(&w, "}");
    }
    wr_str(&w, "]}");
    if (w.err) return TS_ERR_BUF;

    char hdr[TS_HDR_RESERVE];
    wr_t h; wr_init(&h, hdr, sizeof(hdr));
    wr_str(&h, "POST /channels/"); wr_str(&h, channel_id);
    wr_str(&h, "/bulk_update.json HTTP/1.1\r\nHost: " TS_HOST
               "\r\nContent-Type: application/json\r\nConnection: close\r\nContent-Length: ");
    wr_uint(&h, (uint32_t)w.len);
    wr_str(&h, "\r\n\r\n");
    if (h.err) return TS_ERR_BUF;

    char *req = body - h.len;
    memcpy(req, hdr, h.len);
    *start = req;
    return (int)(h.len + w.len);
}

/* ---------- analyse de la réponse ---------- */
static ts_status_t parse_resp(const char *rx, int is_bulk) {
    const char *p = strstr(rx, "HTTP/1.");
    if (!p || strlen(p) < 12) return TS_ERR_HTTP;
    int code = (p[9]-'0')*100 + (p[10]-'0')*10 + (p[11]-'0');
    if (code != 200 && code != 202) return TS_ERR_HTTP;
    if (!is_bulk) {                       /* /update renvoie l'id de l'entrée, 0 = refus */
    	//printf(p);
    	//printf("\r\n");
        const char *body = strstr(p, "\r\n");
        if (!body) return TS_ERR_HTTP;
        body += 4;
        while (*body == '\r' || *body == '\n' || *body == ' ') body++;
        if (*body == '0' && (body[1] < '0' || body[1] > '9')) return TS_ERR_REJECTED;
    }
    return TS_OK;
}

ts_status_t ts_send_update(ts_transport_fn tp, void *ctx,
                           const char *write_key, const ts_sample_t *s) {
    static char req[TS_BUF_SINGLE];       /* non réentrant */
    static char rx[256];
    int n = ts_build_update_get(req, sizeof(req), write_key, s);
    if (n < 0) return TS_ERR_BUF;
    int r = tp(( const uint8_t *)req, (size_t)n, (uint8_t *)rx, sizeof(rx) - 1, 10000, ctx);
    if (r < 0) return TS_ERR_NET;
    rx[r] = 0;
    return parse_resp(rx, 0);
}

ts_status_t ts_send_bulk(ts_transport_fn tp, void *ctx, const char *channel_id,
                         const char *write_key, const ts_bulk_t *b) {
    static char req[TS_BUF_BULK];         /* non réentrant */
    static char rx[256];
    const char *start;
    int n = ts_build_bulk_post(req, sizeof(req), channel_id, write_key, b, &start);
    if (n < 0) return TS_ERR_BUF;
    int r = tp((const uint8_t *)start, (size_t)n, (uint8_t *)rx, sizeof(rx) - 1, 15000, ctx);
    if (r < 0) return TS_ERR_NET;
    rx[r] = 0;
    return parse_resp(rx, 1);
}

/* =====================================================================
 * EXEMPLE (à copier dans main.c)
 * =====================================================================
#if 0
#include "thingspeak.h"

#define WRITE_KEY  "VOTRE_WRITE_API_KEY"
#define CHANNEL_ID "1234567"

// 1) Transport : à adapter à votre module (AT Wi-Fi sur UART, lwIP, modem...)
static int my_transport(const uint8_t *tx, size_t tx_len, uint8_t *rx,
                        size_t rx_max, uint32_t timeout_ms, void *ctx) {
    // a) ouvrir TCP vers api.thingspeak.com:80
    // b) envoyer tx (ex. AT+CIPSEND=<len> puis tx, ou send() lwIP)
    // c) lire la réponse jusqu'à rx_max / timeout
    // d) fermer ; retourner le nb d'octets lus, ou -1
    return -1;
}

// 2) Envoi simple toutes les 10 min
void send_measure(float t, float h, float p, float baro,
                  uint8_t classe, uint8_t conf, float t_pred) {
    ts_sample_t s; ts_sample_init(&s, 0);
    ts_sample_set(&s, 1, t);
    ts_sample_set(&s, 2, h);
    ts_sample_set(&s, 3, p);
    ts_sample_set(&s, 4, baro);
    ts_sample_set(&s, 5, classe);
    ts_sample_set(&s, 6, conf);
    ts_sample_set(&s, 7, t_pred);
    ts_status_t st = ts_send_update(my_transport, NULL, WRITE_KEY, &s);
    // st == TS_ERR_REJECTED : intervalle < 15 s ou clé/licence incorrecte
}

// 3) Envoi groupé : mémoriser N mesures (ex. 1 toutes les 60 s) puis envoyer
static ts_bulk_t bulk;
void on_measure_every_60s(const ts_sample_t *s) {   // s->dt_s = 60
    ts_bulk_add(&bulk, s);
    if (bulk.n == TS_BULK_MAX) {
        if (ts_send_bulk(my_transport, NULL, CHANNEL_ID, WRITE_KEY, &bulk) == TS_OK)
            ts_bulk_clear(&bulk);
    }
}
#endif
*/
