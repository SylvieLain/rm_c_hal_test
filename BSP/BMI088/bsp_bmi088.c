/**
  ******************************************************************************
  * @file    bsp_bmi088.c
  * @brief   BMI088 板级封装（SPI1）
  ******************************************************************************
  */
#include "main.h"
#include "BMI088driver.h"
#include "BMI088Middleware.h"
#include "bsp_bmi088.h"

uint8_t bsp_bmi088_read_accel_id(void)
{
  uint8_t id;

  BMI088_ACCEL_NS_L();
  BMI088_read_write_byte(0x80 | 0x00);   /* ACC_CHIP_ID */
  BMI088_read_write_byte(0x55);          /* dummy */
  id = BMI088_read_write_byte(0x55);
  BMI088_ACCEL_NS_H();

  return id;
}

uint8_t bsp_bmi088_read_gyro_id(void)
{
  uint8_t id;

  BMI088_GYRO_NS_L();
  BMI088_read_write_byte(0x80 | 0x00);   /* GYRO_CHIP_ID */
  id = BMI088_read_write_byte(0x55);
  BMI088_GYRO_NS_H();

  return id;
}

void bsp_bmi088_read(fp32 gyro[3], fp32 accel[3], fp32 *temperature)
{
  BMI088_read(gyro, accel, temperature);
}

uint8_t bsp_bmi088_init(void)
{
  return BMI088_init();
}
