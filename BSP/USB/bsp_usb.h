#ifndef __BSP_USB_H
#define __BSP_USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_def.h"


/* 虚拟串口 bsp */

/* 接收缓冲 2048 字节，满时丢弃最旧数据。主循环读取，返回实际字节数(<=len)。 */
uint16_t bsp_usb_read(uint8_t *data, uint16_t len);
/* 缓冲里还有多少字节待读。 */
uint16_t bsp_usb_rx_available(void);
/* 因缓冲满而丢弃的字节数, 只做诊断。 */
uint32_t bsp_usb_rx_overflow(void);

/* 数据复制后异步发送；USBD_BUSY 时稍后重试。 */
uint8_t bsp_usb_write(const uint8_t *data, uint16_t len);

/* 上位机在线: DTR 置位, 或最近 5 秒内收到过数据。 */
uint8_t bsp_usb_host_online(void);
/* 原始 DTR / RTS 状态。 */
uint8_t bsp_usb_host_dtr(void);
uint8_t bsp_usb_host_rts(void);

/* USB CDC 类回调使用。 */
void bsp_usb_init(uint8_t *tx, uint16_t size);
void bsp_usb_receive(uint8_t *data, uint32_t len);
void bsp_usb_control(uint8_t cmd, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
