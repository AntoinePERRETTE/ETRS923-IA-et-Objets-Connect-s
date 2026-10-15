/**
  ******************************************************************************
  * @file    lps22hh_reg.h
  * @author  Sensors Software Solution Team
  * @brief   This file contains all the functions prototypes for the
  *          lps22hh_reg.c driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LPS22HH_REGS_H
#define LPS22HH_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <math.h>

/** @addtogroup LPS22HH
  * @{
  */

/** @defgroup  Endianness definitions
  * @{
  */

#ifndef DRV_BYTE_ORDER
#ifndef __BYTE_ORDER__

#define DRV_LITTLE_ENDIAN 1234
#define DRV_BIG_ENDIAN    4321

/** if _BYTE_ORDER is not defined, choose the endianness of your architecture
  * by uncommenting the define which fits your platform endianness
  */
/* #define DRV_BYTE_ORDER    DRV_BIG_ENDIAN */
#define DRV_BYTE_ORDER    DRV_LITTLE_ENDIAN

#else /* defined __BYTE_ORDER__ */

#define DRV_LITTLE_ENDIAN  __ORDER_LITTLE_ENDIAN__
#define DRV_BIG_ENDIAN     __ORDER_BIG_ENDIAN__
#define DRV_BYTE_ORDER     __BYTE_ORDER__

#endif /* __BYTE_ORDER__*/
#endif /* DRV_BYTE_ORDER */

/**
  * @}
  */

/** @defgroup STMicroelectronics sensors common types
  * @{
  */

#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t bit0 : 1;
  uint8_t bit1 : 1;
  uint8_t bit2 : 1;
  uint8_t bit3 : 1;
  uint8_t bit4 : 1;
  uint8_t bit5 : 1;
  uint8_t bit6 : 1;
  uint8_t bit7 : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t bit7 : 1;
  uint8_t bit6 : 1;
  uint8_t bit5 : 1;
  uint8_t bit4 : 1;
  uint8_t bit3 : 1;
  uint8_t bit2 : 1;
  uint8_t bit1 : 1;
  uint8_t bit0 : 1;
#endif /* DRV_BYTE_ORDER */
} bitwise_t;

#define PROPERTY_DISABLE (0U)
#define PROPERTY_ENABLE  (1U)

/** @addtogroup  Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  */

typedef int32_t (*stmdev_write_ptr)(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len);
typedef int32_t (*stmdev_read_ptr)(void *handle, uint8_t reg, uint8_t *buf, uint16_t len);
typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

typedef struct
{
  stmdev_write_ptr write_reg;
  stmdev_read_ptr read_reg;
  stmdev_mdelay_ptr mdelay;
  void *handle;
  void *priv_data;
} stmdev_ctx_t;

#endif /* MEMS_SHARED_TYPES */

/** @defgroup LPS22HH_Infos
  * @{
  */

#define LPS22HH_I2C_ADD_H 0xBBU
#define LPS22HH_I2C_ADD_L 0xB9U
#define LPS22HH_ID       0xB3U

/**
  * @}
  */

#define LPS22HH_INTERRUPT_CFG 0x0BU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t pe : 2;
  uint8_t lir : 1;
  uint8_t diff_en : 1;
  uint8_t reset_az : 1;
  uint8_t autozero : 1;
  uint8_t reset_arp : 1;
  uint8_t autorefp : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t autorefp : 1;
  uint8_t reset_arp : 1;
  uint8_t autozero : 1;
  uint8_t reset_az : 1;
  uint8_t diff_en : 1;
  uint8_t lir : 1;
  uint8_t pe : 2;
#endif
} lps22hh_interrupt_cfg_t;

#define LPS22HH_THS_P_L 0x0CU
typedef struct { uint8_t ths : 8; } lps22hh_ths_p_l_t;

#define LPS22HH_THS_P_H 0x0DU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t ths : 7;
  uint8_t not_used_01 : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01 : 1;
  uint8_t ths : 7;
#endif
} lps22hh_ths_p_h_t;

#define LPS22HH_IF_CTRL 0x0EU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t i2c_disable : 1;
  uint8_t i3c_disable : 1;
  uint8_t pd_dis_int1 : 1;
  uint8_t sdo_pu_en : 1;
  uint8_t sda_pu_en : 1;
  uint8_t not_used_01 : 2;
  uint8_t int_en_i3c : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t int_en_i3c : 1;
  uint8_t not_used_01 : 2;
  uint8_t sda_pu_en : 1;
  uint8_t sdo_pu_en : 1;
  uint8_t pd_dis_int1 : 1;
  uint8_t i3c_disable : 1;
  uint8_t i2c_disable : 1;
#endif
} lps22hh_if_ctrl_t;

#define LPS22HH_WHO_AM_I 0x0FU
#define LPS22HH_CTRL_REG1 0x10U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t sim : 1;
  uint8_t bdu : 1;
  uint8_t lpfp_cfg : 2;
  uint8_t odr : 3;
  uint8_t not_used_01 : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01 : 1;
  uint8_t odr : 3;
  uint8_t lpfp_cfg : 2;
  uint8_t bdu : 1;
  uint8_t sim : 1;
#endif
} lps22hh_ctrl_reg1_t;

#define LPS22HH_CTRL_REG2 0x11U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t one_shot : 1;
  uint8_t low_noise_en : 1;
  uint8_t swreset : 1;
  uint8_t not_used_01 : 1;
  uint8_t if_add_inc : 1;
  uint8_t pp_od : 1;
  uint8_t int_h_l : 1;
  uint8_t boot : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t boot : 1;
  uint8_t int_h_l : 1;
  uint8_t pp_od : 1;
  uint8_t if_add_inc : 1;
  uint8_t not_used_01 : 1;
  uint8_t swreset : 1;
  uint8_t low_noise_en : 1;
  uint8_t one_shot : 1;
#endif
} lps22hh_ctrl_reg2_t;

#define LPS22HH_CTRL_REG3 0x12U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t int_s : 2;
  uint8_t drdy : 1;
  uint8_t int_f_ovr : 1;
  uint8_t int_f_wtm : 1;
  uint8_t int_f_full : 1;
  uint8_t not_used_01 : 2;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01 : 2;
  uint8_t int_f_full : 1;
  uint8_t int_f_wtm : 1;
  uint8_t int_f_ovr : 1;
  uint8_t drdy : 1;
  uint8_t int_s : 2;
#endif
} lps22hh_ctrl_reg3_t;

#define LPS22HH_FIFO_CTRL 0x13U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t f_mode : 3;
  uint8_t stop_on_wtm : 1;
  uint8_t not_used_01 : 4;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01 : 4;
  uint8_t stop_on_wtm : 1;
  uint8_t f_mode : 3;
#endif
} lps22hh_fifo_ctrl_t;

#define LPS22HH_FIFO_WTM 0x14U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t wtm : 7;
  uint8_t not_used_01 : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01 : 1;
  uint8_t wtm : 7;
#endif
} lps22hh_fifo_wtm_t;

#define LPS22HH_REF_P_L 0x15U
#define LPS22HH_REF_P_H 0x16U
#define LPS22HH_RPDS_L 0x18U
#define LPS22HH_RPDS_H 0x19U
#define LPS22HH_INT_SOURCE 0x24U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t ph : 1;
  uint8_t pl : 1;
  uint8_t ia : 1;
  uint8_t not_used_01 : 4;
  uint8_t boot_on : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t boot_on : 1;
  uint8_t not_used_01 : 4;
  uint8_t ia : 1;
  uint8_t pl : 1;
  uint8_t ph : 1;
#endif
} lps22hh_int_source_t;

#define LPS22HH_FIFO_STATUS1 0x25U
#define LPS22HH_FIFO_STATUS2 0x26U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t not_used_01 : 5;
  uint8_t fifo_full_ia : 1;
  uint8_t fifo_ovr_ia : 1;
  uint8_t fifo_wtm_ia : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t fifo_wtm_ia : 1;
  uint8_t fifo_ovr_ia : 1;
  uint8_t fifo_full_ia : 1;
  uint8_t not_used_01 : 5;
#endif
} lps22hh_fifo_status2_t;

#define LPS22HH_STATUS 0x27U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t p_da : 1;
  uint8_t t_da : 1;
  uint8_t not_used_01 : 2;
  uint8_t p_or : 1;
  uint8_t t_or : 1;
  uint8_t not_used_02 : 2;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_02 : 2;
  uint8_t t_or : 1;
  uint8_t p_or : 1;
  uint8_t not_used_01 : 2;
  uint8_t t_da : 1;
  uint8_t p_da : 1;
#endif
} lps22hh_status_t;

#define LPS22HH_PRESS_OUT_XL 0x28U
#define LPS22HH_PRESS_OUT_L 0x29U
#define LPS22HH_PRESS_OUT_H 0x2AU
#define LPS22HH_TEMP_OUT_L 0x2BU
#define LPS22HH_TEMP_OUT_H 0x2CU
#define LPS22HH_FIFO_DATA_OUT_PRESS_XL 0x78U
#define LPS22HH_FIFO_DATA_OUT_PRESS_L 0x79U
#define LPS22HH_FIFO_DATA_OUT_PRESS_H 0x7AU
#define LPS22HH_FIFO_DATA_OUT_TEMP_L 0x7BU
#define LPS22HH_FIFO_DATA_OUT_TEMP_H 0x7CU

#ifndef __weak
#define __weak __attribute__((weak))
#endif

int32_t lps22hh_read_reg(const stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);
int32_t lps22hh_write_reg(const stmdev_ctx_t *ctx, uint8_t reg, const uint8_t *data, uint16_t len);

float_t lps22hh_from_lsb_to_hpa(uint32_t lsb);
float_t lps22hh_from_lsb_to_celsius(int16_t lsb);

int32_t lps22hh_block_data_update_set(const stmdev_ctx_t *ctx, uint8_t val);
int32_t lps22hh_block_data_update_get(const stmdev_ctx_t *ctx, uint8_t *val);

typedef enum
{
  LPS22HH_POWER_DOWN = 0x00,
  LPS22HH_ONE_SHOOT = 0x08,
  LPS22HH_1_Hz = 0x01,
  LPS22HH_10_Hz = 0x02,
  LPS22HH_25_Hz = 0x03,
  LPS22HH_50_Hz = 0x04,
  LPS22HH_75_Hz = 0x05,
  LPS22HH_1_Hz_LOW_NOISE = 0x11,
  LPS22HH_10_Hz_LOW_NOISE = 0x12,
  LPS22HH_25_Hz_LOW_NOISE = 0x13,
  LPS22HH_50_Hz_LOW_NOISE = 0x14,
  LPS22HH_75_Hz_LOW_NOISE = 0x15,
  LPS22HH_100_Hz = 0x06,
  LPS22HH_200_Hz = 0x07,
} lps22hh_odr_t;

int32_t lps22hh_data_rate_set(const stmdev_ctx_t *ctx, lps22hh_odr_t val);
int32_t lps22hh_data_rate_get(const stmdev_ctx_t *ctx, lps22hh_odr_t *val);

int32_t lps22hh_pressure_raw_get(const stmdev_ctx_t *ctx, uint32_t *buff);
int32_t lps22hh_temperature_raw_get(const stmdev_ctx_t *ctx, int16_t *buff);

int32_t lps22hh_device_id_get(const stmdev_ctx_t *ctx, uint8_t *buff);

int32_t lps22hh_reset_set(const stmdev_ctx_t *ctx, uint8_t val);
int32_t lps22hh_reset_get(const stmdev_ctx_t *ctx, uint8_t *val);

int32_t lps22hh_auto_increment_set(const stmdev_ctx_t *ctx, uint8_t val);
int32_t lps22hh_auto_increment_get(const stmdev_ctx_t *ctx, uint8_t *val);

int32_t lps22hh_boot_set(const stmdev_ctx_t *ctx, uint8_t val);
int32_t lps22hh_boot_get(const stmdev_ctx_t *ctx, uint8_t *val);

typedef struct
{
  lps22hh_int_source_t int_source;
  lps22hh_fifo_status2_t fifo_status2;
  lps22hh_status_t status;
} lps22hh_all_sources_t;

int32_t lps22hh_all_sources_get(const stmdev_ctx_t *ctx, lps22hh_all_sources_t *val);
int32_t lps22hh_status_reg_get(const stmdev_ctx_t *ctx, lps22hh_status_t *val);

int32_t lps22hh_press_flag_data_ready_get(const stmdev_ctx_t *ctx, uint8_t *val);
int32_t lps22hh_temp_flag_data_ready_get(const stmdev_ctx_t *ctx, uint8_t *val);

#ifdef __cplusplus
}
#endif

#endif /* LPS22HH_REGS_H */
