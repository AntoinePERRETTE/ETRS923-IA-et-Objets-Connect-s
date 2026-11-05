/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ethernet_task.c
  * @author  Antoine PERRETTE
  * @brief   Ethernet ping task implementation
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

/* Private function prototypes -----------------------------------------------*/
static UINT ETHERNET_WaitForDHCP(void);
static void ETHERNET_PrintIPAddress(NX_IP *ip_instance);
static UINT ETHERNET_Ping(NX_IP *ip_instance, ULONG target_ip);

/**
  * @brief  Ethernet ping task entry point
  * @param  thread_input: Thread input parameter
  * @retval None
  */
void ETHERNET_PingTask(ULONG thread_input)
{
  (void)thread_input;

  UINT status;
  char msg[64];
  int size;

  size = sprintf(msg, "\r\n=== Ethernet Ping Task Started ===\r\n");
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  status = ETHERNET_WaitForDHCP();
  if (status != NX_SUCCESS)
  {
    size = sprintf(msg, "DHCP Failed!\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);
    return;
  }

  ETHERNET_PrintIPAddress(&NetXDuoEthIpInstance);

  while (1)
  {
    status = ETHERNET_Ping(&NetXDuoEthIpInstance, PING_TARGET_IP);
    
    if (status == NX_SUCCESS)
    {
      size = sprintf(msg, "Ping to 8.8.8.8 successful!\r\n");
    }
    else
    {
      size = sprintf(msg, "Ping to 8.8.8.8 failed with status: %u\r\n", status);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

    tx_thread_sleep(50);
  }
}

/**
  * @brief  Wait for DHCP to complete
  * @param  None
  * @retval NX_SUCCESS if DHCP succeeded
  */
static UINT ETHERNET_WaitForDHCP(void)
{
  UINT status;

  status = tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER);
  if (status != TX_SUCCESS)
  {
    return NX_DHCP_ERROR;
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

  size = sprintf(msg, "IP Address: %lu.%lu.%lu.%lu\r\n", ip_byte1, ip_byte2, ip_byte3, ip_byte4);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);
}

/**
  * @brief  Ping a target IP address
  * @param  ip_instance: NX_IP instance
  * @param  target_ip: Target IP address in network byte order
  * @retval NX_SUCCESS if ping succeeded
  */
static UINT ETHERNET_Ping(NX_IP *ip_instance, ULONG target_ip)
{
  UINT status;
  NX_PACKET *packet_ptr;
  ULONG actual_status;
  ULONG sequence_number = 0;
  ULONG timestamp;

  status = nx_icmp_ping(ip_instance, target_ip, NULL, 0, &packet_ptr, &actual_status, &sequence_number, &timestamp, NX_IP_PERIODIC_RATE * 5);

  if (packet_ptr)
  {
    nx_packet_release(packet_ptr);
  }

  return status;
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
