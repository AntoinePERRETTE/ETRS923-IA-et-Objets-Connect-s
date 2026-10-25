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

void read_sensors(void);

#endif
