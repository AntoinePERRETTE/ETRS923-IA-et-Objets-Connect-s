/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_netxduo.h"
#include "nx_icmp.h"
extern NX_IP NetXDuoEthIpInstance;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD AppThread;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID AppThread_Entry(ULONG thread_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, 1024, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  ret = tx_thread_create(&AppThread, "AppThread", AppThread_Entry, 0, pointer, 1024, 15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
static VOID AppThread_Entry(ULONG thread_input)
{
  UINT status;
  ULONG ping_address = NX_IP(8, 8, 8, 8);
  ULONG received_packets = 0;
  ULONG sent_packets = 0;
  ULONG round_trip_time = 0;
  ULONG min_round_trip = 0;
  ULONG max_round_trip = 0;

  tx_thread_sleep(50);

  while (1)
  {
    status = nx_icmp_ping(&NetXDuoEthIpInstance, ping_address, 1000, 5, &sent_packets, &received_packets, &round_trip_time, &min_round_trip, &max_round_trip);
    
    if (status == NX_SUCCESS)
    {
      printf("Ping to 8.8.8.8: Sent=%lu, Received=%lu, Avg RTT=%lu ms, Min=%lu ms, Max=%lu ms\n", 
             sent_packets, received_packets, round_trip_time, min_round_trip, max_round_trip);
    }
    else
    {
      printf("Ping failed with status: %u\n", status);
    }
    
    tx_thread_sleep(100);
  }
}
/* USER CODE END 1 */
