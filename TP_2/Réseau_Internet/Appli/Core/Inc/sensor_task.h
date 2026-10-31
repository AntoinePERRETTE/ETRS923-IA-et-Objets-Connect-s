#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "lps22df_reg.h"
#include "stts22h_reg.h"
#include "sht40ad1b_reg.h"
#include "platform.h"
#include <stdio.h>

void SENSORS_startMeasure(void);
void SENSORS_Init(void);

static void SENSORS_LPS22DF_Init(void);
static void SENSORS_STTS22H_Init(void);
static void SENSORS_SHT40AD1B_Init(void);

void SENSORS_LPS22DF_dataGet(void);
void SENSORS_STTS22H_dataGet(void);
void SENSORS_SHT40AD1B_dataGet(void);

#endif
