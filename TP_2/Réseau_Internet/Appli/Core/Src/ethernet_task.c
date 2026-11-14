/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ethernet_task.c
  * @author  Antoine PERRETTE
  * @brief   Ethernet task implementation (deprecated - ping now in app_netxduo)
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

/* Includes ------------------------------------------------------------------*/
#include "ethernet_task.h"
#include "../NetXDuo/App/netxduo_config.h"
#include "tx_api.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/**
  * @brief  Ethernet task entry point (deprecated)
  * @param  thread_input: Thread input parameter
  * @retval None
  */
void ETHERNET_PingTask(ULONG thread_input)
{
  (void)thread_input;

  char msg[64];
  int size;

  size = sprintf(msg, "\r\n=== Ethernet Task Started ===\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  size = sprintf(msg, "Static IP: 192.168.0.100, Netmask: 255.255.255.0\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  size = sprintf(msg, "Ready to respond to ping requests from external PC\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  while (1)
  {
    tx_thread_sleep(100);
  }
}

/**
  * @brief  Initialize Ethernet task
  * @param  None
  * @retval NX_SUCCESS
  */
UINT ETHERNET_Init(void)
{
  return NX_SUCCESS;
}
