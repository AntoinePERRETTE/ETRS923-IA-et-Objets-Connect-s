/**
  ******************************************************************************
  * @file    platform.c
  * @brief   Platform-specific functions for LPS22HH sensor communication via I2C
  ******************************************************************************
  */

#include "platform.h"
#include "stm32n6xx_hal.h"
#include "lps22df_reg.h"
#include "stts22h_reg.h"
#include "sht40ad1b_reg.h"

/**
  * @brief  Platform-specific read function for LPS22DF sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to read
  * @param  buf: Pointer to buffer to store read data
  * @param  len: Number of bytes to read
  * @retval 0 if success, -1 if error
  */
int32_t platform_lps22df_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return platform_I2C_read((I2C_HandleTypeDef*) handle, LPS22DF_I2C_ADD_H, reg, buf, len);
}

/**
  * @brief  Platform-specific write function for LPS22DF sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to write
  * @param  buf: Pointer to data to write
  * @param  len: Number of bytes to write
  * @retval 0 if success, -1 if error
  */
int32_t platform_lps22df_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    return platform_I2C_write((I2C_HandleTypeDef*) handle, LPS22DF_I2C_ADD_H, reg, buf, len);
}

/**
  * @brief  Platform-specific read function for STTS22H sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to read
  * @param  buf: Pointer to buffer to store read data
  * @param  len: Number of bytes to read
  * @retval 0 if success, -1 if error
  */
int32_t platform_stts22h_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
	return platform_I2C_read((I2C_HandleTypeDef*) handle, STTS22H_I2C_ADD_H, reg, buf, len);
}

/**
  * @brief  Platform-specific write function for STTS22H sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to write
  * @param  buf: Pointer to data to write
  * @param  len: Number of bytes to write
  * @retval 0 if success, -1 if error
  */
int32_t platform_stts22h_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    return platform_I2C_write((I2C_HandleTypeDef*) handle, STTS22H_I2C_ADD_H, reg, buf, len);
}

/**
  * @brief  Platform-specific read function for SHT40AD1B sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to read
  * @param  buf: Pointer to buffer to store read data
  * @param  len: Number of bytes to read
  * @retval 0 if success, -1 if error
  */
int32_t platform_sht40ad1b_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
	return platform_I2C_read((I2C_HandleTypeDef*) handle, SHT40AD1B_I2C_ADDRESS, reg, buf, len);
}

/**
  * @brief  Platform-specific write function for SHT40AD1B sensor
  * @param  handle: Pointer to I2C handle (I2C_HandleTypeDef*)
  * @param  reg: Register address to write
  * @param  buf: Pointer to data to write
  * @param  len: Number of bytes to write
  * @retval 0 if success, -1 if error
  */
int32_t platform_sht40ad1b_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    return platform_I2C_write((I2C_HandleTypeDef*) handle, SHT40AD1B_I2C_ADDRESS, reg, buf, len);
}

/**
 * @brief	Platform-specific write function on i2c bus
 * @param 	hi2c: Pointer to I2C handle (I2C_HandleTypeDef*)
 */
int32_t platform_I2C_write(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, const uint8_t *buf, uint16_t len) {
	if (HAL_I2C_Mem_Write(hi2c, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)buf, len, 1000) == HAL_OK)
		return 0;
	else
		return -1;
}

/**
 * @brief	Platform-specific read function on i2c bus
 * @param 	hi2c: Pointer to I2C handle (I2C_HandleTypeDef*)
 */
int32_t platform_I2C_read(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    if (HAL_I2C_Mem_Read(hi2c, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 1000) == HAL_OK)
        return 0;
    else
        return -1;
}

/**
  * @brief  Platform-specific delay function
  * @param  millisec: Delay in milliseconds
  */
void platform_delay(uint32_t millisec)
{
    HAL_Delay(millisec);
}
