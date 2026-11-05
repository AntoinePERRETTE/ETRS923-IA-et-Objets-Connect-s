/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    shell.c
  * @author  Antoine PERRETTE
  * @brief   Simple console shell implementation
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
#include "shell.h"
#include "main.h"
#include "sensor_task.h"
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
static void SHELL_PrintPrompt(void);
static void SHELL_ProcessCommand(char *buffer, uint16_t length);
static void SHELL_PrintSensorsData(void);

/**
  * @brief  Initialize the shell
  * @param  None
  * @retval None
  */
void SHELL_Init(void)
{
  SENSORS_Init();
}

/**
  * @brief  Shell task entry point
  * @param  thread_input: Thread input parameter
  * @retval None
  */
void SHELL_Task(ULONG thread_input)
{
  char buffer[SHELL_BUFFER_SIZE];
  uint16_t buffer_index = 0;
  uint8_t received_char;

  (void)thread_input;

  char welcome_msg[] = "\r\n=== STM32 Shell Console ===\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcome_msg, strlen(welcome_msg), HAL_MAX_DELAY);

  while (1)
  {
    SHELL_PrintPrompt();

    buffer_index = 0;
    while (1)
    {
      if (HAL_UART_Receive(&huart1, &received_char, 1, HAL_MAX_DELAY) == HAL_OK)
      {
        HAL_UART_Transmit(&huart1, &received_char, 1, HAL_MAX_DELAY);

        if (received_char == '\b' || received_char == 127)
        {
          if (buffer_index > 0)
          {
            buffer_index--;
            uint8_t backspace_seq[] = "\b \b";
            HAL_UART_Transmit(&huart1, backspace_seq, 3, HAL_MAX_DELAY);
          }
        }
        else if (received_char == '\r' || received_char == '\n')
        {
          if (buffer_index > 0)
          {
            buffer[buffer_index] = '\0';
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            SHELL_ProcessCommand(buffer, buffer_index);
            break;
          }
          else
          {
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            break;
          }
        }
        else if (buffer_index < SHELL_BUFFER_SIZE - 1)
        {
          buffer[buffer_index++] = received_char;
        }
        else
        {
          buffer[buffer_index] = '\0';
          HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
          SHELL_ProcessCommand(buffer, buffer_index);
          break;
        }
      }
    }
  }
}

/**
  * @brief  Print the shell prompt
  * @param  None
  * @retval None
  */
static void SHELL_PrintPrompt(void)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)SHELL_PROMPT, strlen(SHELL_PROMPT), HAL_MAX_DELAY);
}

/**
  * @brief  Process the received command
  * @param  buffer: Command buffer
  * @param  length: Command length
  * @retval None
  */
static void SHELL_ProcessCommand(char *buffer, uint16_t length)
{
  (void)length;

  if (strcmp(buffer, "get-sensors-data") == 0)
  {
    SHELL_PrintSensorsData();
  }
}

/**
  * @brief  Print sensor data
  * @param  None
  * @retval None
  */
static void SHELL_PrintSensorsData(void)
{
  char msg[64];
  int size;

  SENSORS_dataGet();

  float temperature = SENSORS_getTemperature();
  float humidity = SENSORS_getHumidity();
  float pressure = SENSORS_getPressure();

  size = sprintf(msg, "Temperature : %.2f \u00b0C\r\n", temperature);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  size = sprintf(msg, "Humidity : %.2f %%\r\n", humidity);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);

  size = sprintf(msg, "Pressure : %.2f hPa\r\n", pressure);
  HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, HAL_MAX_DELAY);
}
