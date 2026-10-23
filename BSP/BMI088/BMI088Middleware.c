/**
  ******************************************************************************
  * @file    BMI088Middleware.c
  * @brief   BMI088 底层适配：SPI1 + CS + 延时（无 RTOS）
  *
  *  SPI1: SCK PB3 / MISO PB4 / MOSI PA7 (AF5)
  *  CS  : ACCEL PA4 / GYRO PB0
  ******************************************************************************
  */
#include "BMI088Middleware.h"
#include "main.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

static uint8_t delay_us_ready = 0;

static void delay_us_init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
  delay_us_ready = 1;
}

void BMI088_GPIO_init(void)
{
  /* CS 引脚已在 MX_GPIO_Init() 中配置 */
  BMI088_ACCEL_NS_H();
  BMI088_GYRO_NS_H();
}

void BMI088_com_init(void)
{
  /* SPI1 已在 main() 中通过 MX_SPI1_Init() 配置 */
}

void BMI088_delay_ms(uint16_t ms)
{
  HAL_Delay(ms);
}

void BMI088_delay_us(uint16_t us)
{
  uint32_t start;
  uint32_t ticks;

  if (!delay_us_ready)
  {
    delay_us_init();
  }

  start = DWT->CYCCNT;
  ticks = (uint32_t)us * (SystemCoreClock / 1000000U);
  while ((DWT->CYCCNT - start) < ticks)
  {
  }
}

void BMI088_ACCEL_NS_L(void)
{
  HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}

void BMI088_ACCEL_NS_H(void)
{
  HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}

void BMI088_GYRO_NS_L(void)
{
  HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}

void BMI088_GYRO_NS_H(void)
{
  HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

uint8_t BMI088_read_write_byte(uint8_t txdata)
{
  uint8_t rx_data = 0;
  HAL_SPI_TransmitReceive(&hspi1, &txdata, &rx_data, 1, 1000);
  return rx_data;
}
