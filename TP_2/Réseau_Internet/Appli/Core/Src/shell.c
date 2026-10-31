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
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
static void SHELL_PrintPrompt(void);
static void SHELL_ProcessCommand(char *buffer, uint16_t length);

/* Private defines -----------------------------------------------------------*/

/**
  * @brief  Initialize the shell
  * @param  None
  * @retval None
  */
void SHELL_Init(void)
{
  /* Nothing to initialize for now */
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

  (void)thread_input; /* Unused parameter */

  /* Print welcome message */
  char welcome_msg[] = "\r\n=== STM32 Shell Console ===\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)welcome_msg, strlen(welcome_msg), HAL_MAX_DELAY);

  while (1)
  {
    /* Print prompt */
    SHELL_PrintPrompt();

    /* Read characters until newline or buffer full */
    buffer_index = 0;
    while (1)
    {
      /* Wait for a character */
      if (HAL_UART_Receive(&huart1, &received_char, 1, HAL_MAX_DELAY) == HAL_OK)
      {
        /* Echo the character */
        HAL_UART_Transmit(&huart1, &received_char, 1, HAL_MAX_DELAY);

        /* Check for backspace */
        if (received_char == '\b' || received_char == 127)
        {
          if (buffer_index > 0)
          {
            buffer_index--;
            /* Send backspace, space, backspace to erase character */
            uint8_t backspace_seq[] = "\b \b";
            HAL_UART_Transmit(&huart1, backspace_seq, 3, HAL_MAX_DELAY);
          }
        }
        /* Check for newline or carriage return */
        else if (received_char == '\r' || received_char == '\n')
        {
          if (buffer_index > 0)
          {
            buffer[buffer_index] = '\0';
            /* Print newline */
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            /* Process the command */
            SHELL_ProcessCommand(buffer, buffer_index);
            break;
          }
          else
          {
            /* Just print a new prompt */
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            break;
          }
        }
        /* Check for buffer overflow */
        else if (buffer_index < SHELL_BUFFER_SIZE - 1)
        {
          buffer[buffer_index++] = received_char;
        }
        else
        {
          /* Buffer full, send newline and process */
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
  (void)length; /* Unused parameter */

  /* Simple echo command - just print what was received */
  /* This is the basic echo functionality */
  /* In a real shell, you would parse commands here */
  
  /* For now, we just acknowledge the input */
  char echo_msg[] = "Echo: ";
  HAL_UART_Transmit(&huart1, (uint8_t*)echo_msg, strlen(echo_msg), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}
