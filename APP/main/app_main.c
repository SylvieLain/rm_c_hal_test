/**
  ******************************************************************************
  * @file    app_main.c
  * @brief   RoboMaster C board (STM32F407IGHx) peripheral self-test, bare-metal
  *
  *  UART output : USART1(PA9/PB7) USART2(PD5/PD6) USART3(PC10/PC11)
  *                USART6(PG14/PG9) + USB CDC, all 115200-8-N-1
  *
  *  P0 items   : clock check, UID, reset cause, 1s heartbeat, key/dip switch,
  *               UART echo
  *  P1 items   : BMI088 (SPI1) chip-id + init + accel/gyro/temp print
  ******************************************************************************
  */
#include "main.h"
#include "usart.h"
#include "bsp_uart.h"
#include "bsp_bmi088.h"
#include "app_main.h"

#include <stdio.h>

extern uint32_t SystemCoreClock;

static void print_reset_cause(void)
{
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
  {
    printf("RST: POR/PDR\r\n");
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
  {
    printf("RST: SOFTWARE\r\n");
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
  {
    printf("RST: IWDG\r\n");
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
  {
    printf("RST: WWDG\r\n");
  }
  else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
  {
    printf("RST: LOW-POWER\r\n");
  }
  else
  {
    printf("RST: UNKNOWN\r\n");
  }

  __HAL_RCC_CLEAR_RESET_FLAGS();
}

static void echo_dump(UART_HandleTypeDef *huart, const char *tag)
{
  uint8_t ch;

  while (HAL_UART_Receive(huart, &ch, 1, 0) == HAL_OK)
  {
    printf("[%s RX] 0x%02X '%c'\r\n", tag, ch, (ch >= 0x20 && ch < 0x7F) ? ch : '.');
  }
}

/* BMI088: read chip id, run init (incl. self test), print result */
static void bmi088_test(void)
{
  uint8_t acc_id;
  uint8_t gyro_id;
  uint8_t err;

  acc_id  = bsp_bmi088_read_accel_id();
  gyro_id = bsp_bmi088_read_gyro_id();

  printf("BMI088 ID  : ACC=0x%02X (exp 0x1E) %s | GYRO=0x%02X (exp 0x0F) %s\r\n",
         acc_id,  (acc_id  == 0x1E) ? "PASS" : "FAIL",
         gyro_id, (gyro_id == 0x0F) ? "PASS" : "FAIL");

  err = bsp_bmi088_init();
  printf("BMI088 INIT: 0x%02X %s\r\n", err, (err == 0) ? "PASS" : "FAIL");
}

/* print IMU data as integers to avoid float printf support issues */
static void bmi088_print(void)
{
  fp32 gyro[3] = {0.0f, 0.0f, 0.0f};
  fp32 accel[3] = {0.0f, 0.0f, 0.0f};
  fp32 temp = 0.0f;

  bsp_bmi088_read(gyro, accel, &temp);

  printf("IMU acc[mg]=%ld,%ld,%ld gyro[0.01dps]=%ld,%ld,%ld temp[0.1C]=%ld\r\n",
         (long)(accel[0] * 1000.0f), (long)(accel[1] * 1000.0f), (long)(accel[2] * 1000.0f),
         (long)(gyro[0] * 100.0f),  (long)(gyro[1] * 100.0f),  (long)(gyro[2] * 100.0f),
         (long)(temp * 10.0f));
}

void user_main(void)
{
  uint32_t tick = 0;
  uint32_t last = 0;
  uint32_t last_imu = 0;

  HAL_Delay(50);

  printf("\r\n\r\n");
  printf("==============================================\r\n");
  printf(" RoboMaster C Board  STM32F407IGHx  HAL Test\r\n");
  printf(" build: %s %s\r\n", __DATE__, __TIME__);
  printf("==============================================\r\n");

  printf("UID : %08lX-%08lX-%08lX\r\n",
         (unsigned long)HAL_GetUIDw0(),
         (unsigned long)HAL_GetUIDw1(),
         (unsigned long)HAL_GetUIDw2());

  printf("HSE : %s\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) ? "READY" : "FAIL");
  printf("SYSCLK = %lu Hz (expect 168000000) -> %s\r\n",
         (unsigned long)SystemCoreClock,
         (SystemCoreClock == 168000000U) ? "PASS" : "FAIL");

  printf("HCLK   = %lu Hz\r\n", (unsigned long)HAL_RCC_GetHCLKFreq());
  printf("PCLK1  = %lu Hz\r\n", (unsigned long)HAL_RCC_GetPCLK1Freq());
  printf("PCLK2  = %lu Hz\r\n", (unsigned long)HAL_RCC_GetPCLK2Freq());

  print_reset_cause();
  printf("USART1/USART2/USART3/USART6 @115200-8-N-1\r\n");
  printf("----------------------------------------------\r\n");

  bmi088_test();
  printf("----------------------------------------------\r\n");

  while (1)
  {
    tick = HAL_GetTick();

    /* 1 s heartbeat */
    if ((tick - last) >= 1000U)
    {
      last = tick;

      HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);

      printf("t=%6lu ms  KEY=%d  HW=%d%d%d  LED_B=%s\r\n",
             (unsigned long)tick,
             (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET) ? 1 : 0,
             (HAL_GPIO_ReadPin(HW0_GPIO_Port, HW0_Pin) == GPIO_PIN_SET) ? 1 : 0,
             (HAL_GPIO_ReadPin(HW1_GPIO_Port, HW1_Pin) == GPIO_PIN_SET) ? 1 : 0,
             (HAL_GPIO_ReadPin(HW2_GPIO_Port, HW2_Pin) == GPIO_PIN_SET) ? 1 : 0,
             (HAL_GPIO_ReadPin(LED_B_GPIO_Port, LED_B_Pin) == GPIO_PIN_RESET) ? "ON" : "OFF");
    }

    /* 5 Hz IMU data */
    if ((tick - last_imu) >= 200U)
    {
      last_imu = tick;
      bmi088_print();
    }

    /* UART echo */
    echo_dump(&huart2, "U2");
    echo_dump(&huart1, "U1");
  }
}
