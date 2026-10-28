/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @brief   串口输出
  *          不依赖 RTOS，TX 轮询发送到 USART1/2/3/6
  ******************************************************************************
  */
#include "main.h"
#include "usart.h"
#include "bsp_uart.h"

void bsp_uart_write_all(const uint8_t *data, uint16_t len)
{
  if (data == 0 || len == 0)
  {
    return;
  }

  HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 50);
  HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 50);
  HAL_UART_Transmit(&huart3, (uint8_t *)data, len, 50);
  HAL_UART_Transmit(&huart6, (uint8_t *)data, len, 50);

}

void bsp_uart_puts_all(const char *s)
{
  uint16_t n = 0;
  const char *p = s;

  if (s == 0)
  {
    return;
  }
  while (*p++ != '\0')
  {
    n++;
  }
  bsp_uart_write_all((const uint8_t *)s, n);
}
