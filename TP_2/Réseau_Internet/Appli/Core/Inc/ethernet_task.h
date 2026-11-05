/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ethernet_task.h
  * @author  Antoine PERRETTE
  * @brief   Ethernet ping task header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETHERNET_TASK_H
#define __ETHERNET_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"
#include "../NetXDuo/App/netxduo_config.h"

/* Exported constants --------------------------------------------------------*/
#define ETHERNET_THREAD_STACK_SIZE  2048
#define ETHERNET_THREAD_PRIORITY    6
#define PING_TARGET_IP            0x08080808  // 8.8.8.8

/* Exported functions prototypes ---------------------------------------------*/
void ETHERNET_PingTask(ULONG thread_input);
UINT ETHERNET_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ETHERNET_TASK_H */
