/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_netxduo.c
 * @author  MCD Application Team
 * @brief   NetXDuo applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "nxd_dhcp_client.h"
#include "nxd_dns.h"
/* USER CODE BEGIN Includes */
#include "sensor_task.h"
#include "thingspeak.h"
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
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
TX_SEMAPHORE   DHCPSemaphore;
NX_DHCP        DHCPClient;
/* USER CODE BEGIN PV */
TX_THREAD AppTCPThread;
TX_THREAD AppLinkThread;

NX_TCP_SOCKET TCPSocket;

ULONG          IpAddress;
ULONG          NetMask;

NX_DNS DnsClient;
static ULONG ts_server_ip;
#define TS_WRITE_KEY "E3QHXZ4C2TUDIC2L"
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);
/* USER CODE BEGIN PFP */
static VOID App_TCP_Thread_Entry(ULONG thread_input);
static VOID App_Link_Thread_Entry(ULONG thread_input);
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
	CHAR *pointer;

	/* USER CODE BEGIN MX_NetXDuo_MEM_POOL */

	/* USER CODE END MX_NetXDuo_MEM_POOL */
	/* USER CODE BEGIN 0 */
	printf("Nx_TCP_Echo_Client application started..\r\n");
	/* USER CODE END 0 */

	/* Initialize the NetXDuo system. */
	nx_system_initialize();

	/* Allocate the memory for packet_pool.  */
	if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		return TX_POOL_ERROR;
	}

	/* Create the Packet pool to be used for packet allocation,
	 * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
	 */
	ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

	if (ret != NX_SUCCESS)
	{
		return NX_POOL_ERROR;
	}

	/* Allocate the memory for Ip_Instance */
	if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		return TX_POOL_ERROR;
	}

	/* Create the main NX_IP instance */
	ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
			pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

	if (ret != NX_SUCCESS)
	{
		return NX_NOT_SUCCESSFUL;
	}

	/* Allocate the memory for ARP */
	if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		return TX_POOL_ERROR;
	}

	/* Enable the ARP protocol and provide the ARP cache size for the IP instance */

	/* USER CODE BEGIN ARP_Protocol_Initialization */

	/* USER CODE END ARP_Protocol_Initialization */

	ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

	if (ret != NX_SUCCESS)
	{
		return NX_NOT_SUCCESSFUL;
	}

	/* Enable the ICMP */

	/* USER CODE BEGIN ICMP_Protocol_Initialization */

	/* USER CODE END ICMP_Protocol_Initialization */

	ret = nx_icmp_enable(&NetXDuoEthIpInstance);

	if (ret != NX_SUCCESS)
	{
		return NX_NOT_SUCCESSFUL;
	}

	/* Enable TCP Protocol */

	/* USER CODE BEGIN TCP_Protocol_Initialization */
	/* Allocate the memory for TCP server thread   */
	if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_TCP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
		{
			return TX_POOL_ERROR;
		}

		/* create the TCP server thread */
		ret = tx_thread_create(&AppTCPThread, "App TCP Thread", App_TCP_Thread_Entry, 0, pointer, APP_TCP_THREAD_STACK_SIZE,
				NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);

		if (ret != TX_SUCCESS)
		{
			return NX_NOT_SUCCESSFUL;
		}
		/* USER CODE END TCP_Protocol_Initialization */

		ret = nx_tcp_enable(&NetXDuoEthIpInstance);

		if (ret != NX_SUCCESS)
		{
			return NX_NOT_SUCCESSFUL;
		}

		/* Enable the UDP protocol required for  DHCP communication */

		/* USER CODE BEGIN UDP_Protocol_Initialization */

		/* USER CODE END UDP_Protocol_Initialization */

		ret = nx_udp_enable(&NetXDuoEthIpInstance);

		if (ret != NX_SUCCESS)
		{
			return NX_NOT_SUCCESSFUL;
		}

		/* Allocate the memory for main thread   */
		if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
		{
			return TX_POOL_ERROR;
		}

		/* Create the main thread */
		ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
				NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

		if (ret != TX_SUCCESS)
		{
			return TX_THREAD_ERROR;
		}

		/* Create the DHCP client */

		/* USER CODE BEGIN DHCP_Protocol_Initialization */

		/* USER CODE END DHCP_Protocol_Initialization */

		ret = nx_dhcp_create(&DHCPClient, &NetXDuoEthIpInstance, "DHCP Client");

		if (ret != NX_SUCCESS)
		{
			return NX_DHCP_ERROR;
		}

		/* set DHCP notification callback  */
		tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);

		/* USER CODE BEGIN MX_NetXDuo_Init */
		/* Allocate the memory for Link thread   */
		if (tx_byte_allocate(byte_pool, (VOID **) &pointer,NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
		{
			return TX_POOL_ERROR;
		}

		/* create the Link thread */
		ret = tx_thread_create(&AppLinkThread, "App Link Thread", App_Link_Thread_Entry, 0, pointer, NX_APP_THREAD_STACK_SIZE,
				LINK_PRIORITY, LINK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

		if (ret != TX_SUCCESS)
		{
			return TX_THREAD_ERROR;
		}
		/* USER CODE END MX_NetXDuo_Init */

		return ret;
}

/**
 * @brief  ip address change callback.
 * @param ip_instance: NX_IP instance
 * @param ptr: user data
 * @retval none
 */
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
	/* USER CODE BEGIN ip_address_change_notify_callback */
	/* release the semaphore as soon as an IP address is available */
	if (nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask) != NX_SUCCESS)
	{
		/* USER CODE BEGIN IP address change callback error */
		Error_Handler();
		/* USER CODE END IP address change callback error */
	}
	if(IpAddress != NULL_ADDRESS)
	{
		tx_semaphore_put(&DHCPSemaphore);
	}
	/* USER CODE END ip_address_change_notify_callback */
}

/**
 * @brief  Main thread entry.
 * @param thread_input: ULONG user argument used by the thread entry
 * @retval none
 */
static VOID nx_app_thread_entry (ULONG thread_input)
{
	/* USER CODE BEGIN Nx_App_Thread_Entry 0 */

	/* USER CODE END Nx_App_Thread_Entry 0 */

	UINT ret = NX_SUCCESS;

	/* USER CODE BEGIN Nx_App_Thread_Entry 1 */

	/* USER CODE END Nx_App_Thread_Entry 1 */

	/* register the IP address change callback */
	ret = nx_ip_address_change_notify(&NetXDuoEthIpInstance, ip_address_change_notify_callback, NULL);
	if (ret != NX_SUCCESS)
	{
		/* USER CODE BEGIN IP address change callback error */
		Error_Handler();
		/* USER CODE END IP address change callback error */
	}

	/* start the DHCP client */
	ret = nx_dhcp_start(&DHCPClient);
	if (ret != NX_SUCCESS)
	{
		/* USER CODE BEGIN DHCP client start error */
		Error_Handler();
		/* USER CODE END DHCP client start error */
	}
	printf("Looking for DHCP server ..\r\n");
	/* wait until an IP address is ready */
	if(tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
	{
		/* USER CODE BEGIN DHCPSemaphore get error */
		Error_Handler();
		/* USER CODE END DHCPSemaphore get error */
	}

	/* USER CODE BEGIN Nx_App_Thread_Entry 2 */

	PRINT_IP_ADDRESS(IpAddress);

	/* the network is correctly initialized, start the TCP server thread */
	tx_thread_resume(&AppTCPThread);

	/* if this thread is not needed any more, we relinquish it */
	tx_thread_relinquish();

	return;
	/* USER CODE END Nx_App_Thread_Entry 2 */

}
/* USER CODE BEGIN 1 */
static int ts_transport(const uint8_t *tx, size_t tx_len, uint8_t *rx,
		size_t rx_max, uint32_t timeout_ms, void *ctx)
{
	NX_TCP_SOCKET sock;
	NX_PACKET *pkt;
	ULONG tmo = (timeout_ms * NX_IP_PERIODIC_RATE) / 1000;
	size_t total = 0;
	int result = -1;

	if (nx_tcp_socket_create(&NetXDuoEthIpInstance, &sock, "TS socket", NX_IP_NORMAL,
			NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, WINDOW_SIZE,
			NX_NULL, NX_NULL) != NX_SUCCESS) return -1;

	if (nx_tcp_client_socket_bind(&sock, NX_ANY_PORT, tmo) != NX_SUCCESS) goto del;
	if (nx_tcp_client_socket_connect(&sock, ts_server_ip, 80, tmo) != NX_SUCCESS) goto unbind;

	if (nx_packet_allocate(&NxAppPool, &pkt, NX_TCP_PACKET, tmo) != NX_SUCCESS) goto disc;
	if (nx_packet_data_append(pkt, (VOID *)tx, tx_len, &NxAppPool, tmo) != NX_SUCCESS)
	{ nx_packet_release(pkt); goto disc; }
	if (nx_tcp_socket_send(&sock, pkt, tmo) != NX_SUCCESS)
	{ nx_packet_release(pkt); goto disc; }   /* on success NetX owns the packet */

	/* read until the server closes the connection (Connection: close) or buffer full */
	while (total < rx_max && nx_tcp_socket_receive(&sock, &pkt, tmo) == NX_SUCCESS)
	{
		ULONG n = 0;
		nx_packet_data_extract_offset(pkt, 0, rx + total, rx_max - total, &n);
		nx_packet_release(pkt);
		total += n;
	}
	result = (int)total;

	disc:   nx_tcp_socket_disconnect(&sock, tmo);
	unbind: nx_tcp_client_socket_unbind(&sock);
	del:    nx_tcp_socket_delete(&sock);
	return result;
}

/**
 * @brief  TCP thread entry.
 * @param thread_input: thread user data
 * @retval none
 */

static VOID App_TCP_Thread_Entry(ULONG thread_input)
{
	SENSORS_Init();

	printf("Creating DNS client \r\n");

	nx_dns_create(&DnsClient, &NetXDuoEthIpInstance, (UCHAR *)"DNS Client");
	nx_dns_packet_pool_set(&DnsClient, &NxAppPool);

	nx_dns_server_add(&DnsClient, IP_ADDRESS(8, 8, 8, 8));   /* or use the DHCP-provided DNS */

	printf("Looking for DNS host name \r\n");

	if (nx_dns_host_by_name_get(&DnsClient, (UCHAR *)"api.thingspeak.com",
			&ts_server_ip, 5 * NX_IP_PERIODIC_RATE) != NX_SUCCESS)
	{
		printf("Failed to get dns host name !\r\n");
		Error_Handler();
	}

	while (1)
	{
		SENSORS_dataGet();

		ts_sample_t s;
		ts_sample_init(&s, 0);
		ts_sample_set(&s, 1, SENSORS_getTemperature());
		ts_sample_set(&s, 2, SENSORS_getHumidity());
		ts_sample_set(&s, 3, SENSORS_getPressure());

		ts_status_t st = ts_send_update(ts_transport, NULL, TS_WRITE_KEY, &s);
		printf("ThingSpeak status: %d\r\n", (int)st);

		tx_thread_sleep(1200 * TX_TIMER_TICKS_PER_SECOND);   /* free tier: min 15 s */
	}
}

/**
 * @brief  Link thread entry
 * @param thread_input: ULONG thread parameter
 * @retval none
 */
static VOID App_Link_Thread_Entry(ULONG thread_input)
{
	ULONG actual_status;
	UINT linkdown = 0, status;

	while(1)
	{
		/* Send request to check if the Ethernet cable is connected. */
		status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_LINK_ENABLED,
				&actual_status, 10);

		if(status == NX_SUCCESS)
		{
			if(linkdown == 1)
			{
				linkdown = 0;

				/* The network cable is connected. */
				printf("The network cable is connected.\n");

				/* Send request to enable PHY Link. */
				nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_ENABLE,
						&actual_status);

				/* Send request to check if an address is resolved. */
				status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_ADDRESS_RESOLVED,
						&actual_status, 10);
				if(status == NX_SUCCESS)
				{
					/* Stop DHCP */
					nx_dhcp_stop(&DHCPClient);

					/* Reinitialize DHCP */
					nx_dhcp_reinitialize(&DHCPClient);

					/* Start DHCP */
					nx_dhcp_start(&DHCPClient);

					/* wait until an IP address is ready */
					if(tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
					{
						/* USER CODE BEGIN DHCPSemaphore get error */
						Error_Handler();
						/* USER CODE END DHCPSemaphore get error */
					}

					PRINT_IP_ADDRESS(IpAddress);
				}
				else
				{
					/* Set the DHCP Client's remaining lease time to 0 seconds to trigger an immediate renewal request for a DHCP address. */
					nx_dhcp_client_update_time_remaining(&DHCPClient, 0);
				}
			}
		}
		else
		{
			if(0 == linkdown)
			{
				linkdown = 1;
				/* The network cable is not connected. */
				printf("The network cable is not connected.\r\n");
				nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_DISABLE,
						&actual_status);
			}
		}

		tx_thread_sleep(NX_APP_CABLE_CONNECTION_CHECK_PERIOD);
	}
}

/* USER CODE END 1 */
