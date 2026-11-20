#ifndef METEO_NORM_H
#define METEO_NORM_H

/* Genere par Colab : pretraitement identique a l'entrainement */
#define METEO_W          12      /* heures dans la fenetre (valeurs horaires, UTC) */
#define METEO_C          6      /* T, HR, P_mer, T-Td, sin(h), cos(h) */
#define METEO_NCLS       4
#define METEO_INPUT_NCHW 1      /* ordre du buffer d'entree : index = c*METEO_W + t */

static const float METEO_MU[METEO_C]    = { 12.790601f, 72.583786f, 1023.329773f, 5.522900f, -0.002132f, 0.004352f };
static const float METEO_SIGMA[METEO_C] = { 8.201093f, 18.649912f, 9.483298f, 4.739449f, 0.707034f, 0.709737f };
static const char *const METEO_CLASSES[METEO_NCLS] = { "Clair", "Nuageux", "Brouillard", "Pluie" };

#endif
