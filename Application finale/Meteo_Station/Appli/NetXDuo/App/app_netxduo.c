/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
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
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nxd_dhcp_client.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_PAYLOAD_SIZE        1536    /* = RxBuffLen de ETH1 */
#define NX_APP_PACKET_POOL_SIZE     ((DEFAULT_PAYLOAD_SIZE + sizeof(NX_PACKET)) * 10)
#define NX_IP_THREAD_STACK_SIZE     (2 * 1024)
#define NX_APP_THREAD_STACK_SIZE    (2 * 1024)
#define DEFAULT_ARP_CACHE_SIZE      1024
#define NX_APP_THREAD_PRIORITY      10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* noms utilises par meteo_app.c */
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;

static TX_THREAD    NxAppThread;
static TX_SEMAPHORE DHCPSemaphore;
static NX_DHCP      DHCPClient;
static ULONG        IpAddress, NetMask;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID nx_app_thread_entry(ULONG thread_input);
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);
extern void METEO_App_Start(void);          /* meteo_app.c */

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END MX_NetXDuo_MEM_POOL */
  /* USER CODE BEGIN 0 */

  /* USER CODE END 0 */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  CHAR *pointer;

  nx_system_initialize();

  /* Pool de paquets (dans le pool NetX, zone non cacheable 0x341F8800) */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    return TX_POOL_ERROR;
  if (nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE,
                            pointer, NX_APP_PACKET_POOL_SIZE) != NX_SUCCESS)
    return NX_POOL_ERROR;

  /* Instance IP (adresse 0 : elle sera donnee par le DHCP) */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_IP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    return TX_POOL_ERROR;
  if (nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", 0, 0, &NxAppPool,
                   nx_stm32_eth_driver, pointer, NX_IP_THREAD_STACK_SIZE,
                   NX_APP_THREAD_PRIORITY) != NX_SUCCESS)
    return NX_NOT_SUCCESSFUL;

  /* Protocoles : ARP, ICMP (ping), TCP (ThingSpeak), UDP (DHCP, DNS) */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    return TX_POOL_ERROR;
  if (nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE) != NX_SUCCESS)
    return NX_NOT_SUCCESSFUL;
  if (nx_icmp_enable(&NetXDuoEthIpInstance) != NX_SUCCESS) return NX_NOT_SUCCESSFUL;
  if (nx_tcp_enable(&NetXDuoEthIpInstance)  != NX_SUCCESS) return NX_NOT_SUCCESSFUL;
  if (nx_udp_enable(&NetXDuoEthIpInstance)  != NX_SUCCESS) return NX_NOT_SUCCESSFUL;

  /* Thread qui attend l'adresse IP puis lance l'application meteo */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    return TX_POOL_ERROR;
  if (tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry, 0,
                       pointer, NX_APP_THREAD_STACK_SIZE,
                       NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    return TX_THREAD_ERROR;

  /* Client DHCP */
  if (nx_dhcp_create(&DHCPClient, &NetXDuoEthIpInstance, "DHCP Client") != NX_SUCCESS)
    return NX_DHCP_ERROR;
  tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
/* Appele par NetX quand l'adresse IP change : debloque le thread */
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
  (void)ip_instance; (void)ptr;
  if (nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask) == NX_SUCCESS &&
      IpAddress != 0)
  {
    tx_semaphore_put(&DHCPSemaphore);
  }
}

static VOID nx_app_thread_entry(ULONG thread_input)
{
  (void)thread_input;

  nx_ip_address_change_notify(&NetXDuoEthIpInstance, ip_address_change_notify_callback, NULL);

  if (nx_dhcp_start(&DHCPClient) != NX_SUCCESS)
  {
    printf("[NET] Erreur demarrage DHCP\r\n");
    return;
  }
  printf("[NET] Recherche du serveur DHCP...\r\n");
  tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER);

  printf("[NET] Adresse IP : %lu.%lu.%lu.%lu\r\n",
         (IpAddress >> 24) & 0xff, (IpAddress >> 16) & 0xff,
         (IpAddress >> 8) & 0xff, IpAddress & 0xff);

  METEO_App_Start();          /* capteurs + IA + ThingSpeak */
}

/* USER CODE END 1 */
