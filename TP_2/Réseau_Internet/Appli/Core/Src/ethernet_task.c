/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ethernet_task.c
  * @author  Antoine PERRETTE
  * @brief   Ethernet task implementation for ping response
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
#include "nx_icmpv4.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Private defines -----------------------------------------------------------*/
#define STATIC_IP_ADDRESS   0xC0A80164  // 192.168.1.100
#define STATIC_NET_MASK    0xFFFFFF00  // 255.255.255.0
#define STATIC_GATEWAY     0xC0A80101  // 192.168.1.1

/* Private function prototypes -----------------------------------------------*/
static UINT ETHERNET_ConfigureStaticIP(NX_IP *ip_instance);
static void ETHERNET_PrintIPAddress(NX_IP *ip_instance);

/**
  * @brief  Ethernet task entry point
  * @param  thread_input: Thread input parameter
  * @retval None
  */
void ETHERNET_PingTask(ULONG thread_input)
{
  (void)thread_input;

  UINT status;
  char msg[64];
  int size;

  size = sprintf(msg, "\r\n=== Ethernet Task Started ===\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  size = sprintf(msg, "Configuring static IP address...\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  status = ETHERNET_ConfigureStaticIP(&NetXDuoEthIpInstance);
  if (status != NX_SUCCESS)
  {
    size = sprintf(msg, "Failed to configure static IP!\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);
    return;
  }

  tx_thread_sleep(20);

  ETHERNET_PrintIPAddress(&NetXDuoEthIpInstance);

  size = sprintf(msg, "Ready to respond to ping requests from external PC\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  while (1)
  {
    tx_thread_sleep(100);
  }
}

/**
  * @brief  Configure static IP address
  * @param  ip_instance: NX_IP instance
  * @retval NX_SUCCESS if configuration succeeded
  */
static UINT ETHERNET_ConfigureStaticIP(NX_IP *ip_instance)
{
  UINT status;

  status = nx_ip_address_set(ip_instance, STATIC_IP_ADDRESS, STATIC_NET_MASK);
  if (status != NX_SUCCESS)
  {
    return status;
  }

  status = nx_ip_gateway_address_set(ip_instance, STATIC_GATEWAY);
  if (status != NX_SUCCESS)
  {
    return status;
  }

  return NX_SUCCESS;
}

/**
  * @brief  Print the IP address
  * @param  ip_instance: NX_IP instance
  * @retval None
  */
static void ETHERNET_PrintIPAddress(NX_IP *ip_instance)
{
  ULONG ip_address;
  ULONG net_mask;
  char msg[64];
  int size;

  nx_ip_address_get(ip_instance, &ip_address, &net_mask);

  ULONG ip_byte1 = (ip_address >> 24) & 0xFF;
  ULONG ip_byte2 = (ip_address >> 16) & 0xFF;
  ULONG ip_byte3 = (ip_address >> 8) & 0xFF;
  ULONG ip_byte4 = ip_address & 0xFF;

  size = sprintf(msg, "Static IP Address: %lu.%lu.%lu.%lu\r\n", ip_byte1, ip_byte2, ip_byte3, ip_byte4);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);
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
