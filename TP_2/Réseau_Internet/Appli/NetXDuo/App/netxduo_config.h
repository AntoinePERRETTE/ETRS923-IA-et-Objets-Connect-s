/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    netxduo_config.h
  * @author  Antoine PERRETTE
  * @brief   NetXDuo configuration header with external declarations
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
#ifndef __NETXDUO_CONFIG_H
#define __NETXDUO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"
#include "nxd_dhcp_client.h"

/* Exported variables --------------------------------------------------------*/
extern NX_IP NetXDuoEthIpInstance;
extern NX_DHCP DHCPClient;
extern TX_SEMAPHORE DHCPSemaphore;

#ifdef __cplusplus
}
#endif

#endif /* __NETXDUO_CONFIG_H */
