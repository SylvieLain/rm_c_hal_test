#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 向全部已初始化串口发送（USART1/2/3/6），任一口接 CH340 都能看到 */
void bsp_uart_write_all(const uint8_t *data, uint16_t len);

/* 发送字符串 */
void bsp_uart_puts_all(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
