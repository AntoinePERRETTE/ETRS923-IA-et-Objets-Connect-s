/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    shell.h
  * @author  Antoine PERRETTE
  * @brief   Simple console shell header file
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
#ifndef __SHELL_H
#define __SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define SHELL_THREAD_STACK_SIZE  1024
#define SHELL_THREAD_PRIORITY    4

#define SHELL_PROMPT "STM32> "
#define SHELL_BUFFER_SIZE  64

/* Exported functions prototypes ---------------------------------------------*/
void SHELL_Init(void);
void SHELL_Task(ULONG thread_input);

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H */
