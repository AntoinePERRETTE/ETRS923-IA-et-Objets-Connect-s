/**
  ******************************************************************************
  * @file    platform.h
  * @brief   Platform-specific header for LPS22HH sensor communication
  ******************************************************************************
  */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "lps22df_reg.h"
#include "stm32n6xx_hal.h"

// Fonctions de lecture/écriture bas niveau
int32_t platform_lps22df_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len);
int32_t platform_lps22df_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len);
int32_t platform_stts22h_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len);
int32_t platform_stts22h_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len);
int32_t platform_sht40ad1b_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len);
int32_t platform_sht40ad1b_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len);

int32_t platform_I2C_Transmit(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, const uint8_t *buf, uint16_t len);
int32_t platform_I2C_Receive(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, const uint8_t *buf, uint16_t len);
int32_t platform_I2C_write(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, const uint8_t *buf, uint16_t len);
int32_t platform_I2C_read(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len);
void platform_delay(uint32_t millisec);

#endif /* PLATFORM_H */
