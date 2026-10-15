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
  *
  */

/** @defgroup  Endianness definitions
  * @{
  *
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
  *
  */

/** @defgroup STMicroelectronics sensors common types
  * @{
  *
  */

#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
 
 uint8_t bit0       : 1;
  uint8_t bit1       : 1;
  uint8_t bit2       : 1;
  uint8_t bit3       : 1;
  uint8_t bit4       : 1;
  uint8_t bit5       : 1;
  uint8_t bit6       : 1;
  uint8_t bit7       : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t bit7       : 1;
  uint8_t bit6       : 1;
  uint8_t bit5       : 1;
  uint8_t bit4       : 1;
  uint8_t bit3       : 1;
  uint8_t bit2       : 1;
  uint8_t bit1       : 1;
  uint8_t bit0       : 1;
#endif /* DRV_BYTE_ORDER */
} bitwise_t;

#define PROPERTY_DISABLE                (0U)
#define PROPERTY_ENABLE                 (1U)

/** @addtogroup  Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */

typedef int32_t (*stmdev_write_ptr)(
    void *handle,
    uint8_t reg,
    const uint8_t *buf,
    uint16_t len);

typedef int32_t (*stmdev_read_ptr)(
    void *handle,
    uint8_t reg,
    uint8_t *buf,
    uint16_t len);

typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

typedef struct
{
  /** Component mandatory fields **/
  stmdev_write_ptr  write_reg;
  stmdev_read_ptr   read_reg;
  /** Component optional fields **/
  stmdev_mdelay_ptr   mdelay;
  /** Customizable optional pointer **/
  void *handle;

  /** private data **/
  void *priv_data;
} stmdev_ctx_t;

/**
  * @}
  *
  */

#endif /* MEMS_SHARED_TYPES */

/** @defgroup LPS22HH_Infos
  * @{
  *
  */

/** I2C Device Address 8 bit format  if SA0=0 -> B9 if SA0=1 -> BB **/
#define LPS22HH_I2C_ADD_H                       0xBBU
#define LPS22HH_I2C_ADD_L                       0xB9U

/** Device Identification (Who am I) **/
#define LPS22HH_ID                              0xB3U

/**
  * @}
  *
  */

#define LPS22HH_INTERRUPT_CFG                   0x0BU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t pe                              : 2;  /* ple + phe */
  uint8_t lir                             : 1;
  uint8_t diff_en                         : 1;
  uint8_t reset_az                        : 1;
  uint8_t autozero                        : 1;
  uint8_t reset_arp                       : 1;
  uint8_t autorefp                        : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t autorefp                        : 1;
  uint8_t reset_arp                       : 1;
  uint8_t autozero                        : 1;
  uint8_t reset_az                        : 1;
  uint8_t diff_en                         : 1;
  uint8_t lir                             : 1;
  uint8_t pe                              : 2;  /* ple + phe */
#endif /* DRV_BYTE_ORDER */
} lps22hh_interrupt_cfg_t;

#define LPS22HH_THS_P_L                         0x0CU
typedef struct
{
  uint8_t ths
                             : 8;
} lps22hh_ths_p_l_t;

#define LPS22HH_THS_P_H                         0x0DU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t ths                             : 7;
  uint8_t not_used_01                     : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01
                     : 1;
  uint8_t ths                             : 7;
#endif /* DRV_BYTE_ORDER */
} lps22hh_ths_p_h_t;

#define LPS22HH_IF_CTRL                         0x0EU
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t i2c_disable                     : 1;
  uint8_t i3c_disable                     : 1;
  uint8_t pd_dis_int1                     : 1;
  uint8_t sdo_pu_en                       : 1;
  uint8_t sda_pu_en                       : 1;
  uint8_t not_used_01
                     : 2;
  uint8_t int_en_i3c
                      : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t int_en_i3c                      : 1;
  uint8_t not_used_01                     : 2;
  uint8_t sda_pu_en                       : 1;
  uint8_t sdo_pu_en                       : 1;
  uint8_t pd_dis_int1                     : 1;
  uint8_t i3c_disable                     : 1;
  uint8_t i2c_disable                     : 1;
#endif /* DRV_BYTE_ORDER */
} lps22hh_if_ctrl_t;

#define LPS22HH_WHO_AM_I                        0x0FU
#define LPS22HH_CTRL_REG1                       0x10U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t sim
                             : 1;
  uint8_t bdu                             : 1;
  uint8_t lpfp_cfg                        : 2;  /* en_lpfp + lpfp_cfg */
  uint8_t odr                             : 3;
  uint8_t not_used_01
                     : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used_01                     : 1;
  uint8_t odr
                             : 3;
  uint8_t lpfp_cfg                        : 2;  /* en_lpfp + lpfp_cfg */
  uint8_t bdu                             : 1;
  uint8_t sim
                             : 1;
#endif /* DRV_BYTE_ORDER */
} lps22hh_ctrl_reg1_t;

#define LPS22HH_CTRL_REG2
                       0x11U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t one_shot
                        : 1;
  uint8_t low_noise_en
                    : 1;
  uint8_t swreset
                         : 1;
  uint8_t not_used_01
                     : 1;
  uint8_t if_add_inc
                      : 1;
  uint8_t pp_od
                           : 1;
  uint8_t int_h_l
                         : 1;
  uint8_t boot
                            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t boot
                            : 1;
  uint8_t int_h_l
                         : 1;
  uint8_t pp_od
                           : 1;
  uint8_t if_add_inc
                      : 1;
  uint8_t not_used_01                     : 1;
  uint8_t swreset
