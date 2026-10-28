#include "bsp_usb.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include <string.h>
#include "struct_typedef.h"

static uint8_t rx_ring[2048];
static ring_t usb_ring = {rx_ring, sizeof(rx_ring), 0, 0};
static volatile uint32_t rx_overflow;
static uint8_t *tx_data;
static uint16_t tx_size;
static uint8_t line_coding[7] = {0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08};
static volatile uint8_t host_lines;
static volatile uint8_t rx_seen;
static volatile uint32_t last_rx;

void bsp_usb_init(uint8_t *tx, uint16_t size)
{
  usb_ring.head = 0;
  usb_ring.tail = 0;
  rx_overflow = 0;
  host_lines = 0;
  rx_seen = 0;
  last_rx = 0;
  tx_data = tx;
  tx_size = size;
}

void bsp_usb_receive(uint8_t *data, uint32_t len)
{
  for (uint32_t i = 0; i < len; i++)
  {
    if ((uint16_t)(usb_ring.head - usb_ring.tail) == usb_ring.size)
    {
      usb_ring.tail++;
      rx_overflow++;
    }
    usb_ring.buf[usb_ring.head & (usb_ring.size - 1)] = data[i];
    usb_ring.head++;
  }
  if (len != 0)
  {
    last_rx = HAL_GetTick();
    rx_seen = 1;
  }
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
}

void bsp_usb_control(uint8_t cmd, uint8_t *data, uint16_t len)
{
  if (cmd == CDC_SET_LINE_CODING && len == sizeof(line_coding))
  {
    memcpy(line_coding, data, sizeof(line_coding));
  }
  else if (cmd == CDC_GET_LINE_CODING)
  {
    memcpy(data, line_coding, sizeof(line_coding));
  }
  else if (cmd == CDC_SET_CONTROL_LINE_STATE)
  {
    host_lines = (uint8_t)((USBD_SetupReqTypedef *)data)->wValue;
  }
}

uint16_t bsp_usb_read(uint8_t *data, uint16_t len)
{
  if (data == NULL || len == 0)
  {
    return 0;
  }

  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  uint16_t available = (uint16_t)(usb_ring.head - usb_ring.tail);
  if (len > available)
  {
    len = available;
  }
  uint16_t offset = usb_ring.tail & (usb_ring.size - 1);
  uint16_t first = usb_ring.size - offset;
  if (first > len)
  {
    first = len;
  }
  memcpy(data, usb_ring.buf + offset, first);
  memcpy(data + first, usb_ring.buf, len - first);
  usb_ring.tail += len;
  __set_PRIMASK(primask);
  return len;
}

uint16_t bsp_usb_rx_available(void)
{
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  uint16_t available = (uint16_t)(usb_ring.head - usb_ring.tail);
  __set_PRIMASK(primask);
  return available;
}

uint32_t bsp_usb_rx_overflow(void)
{
  return rx_overflow;
}

uint8_t bsp_usb_write(const uint8_t *data, uint16_t len)
{
  if (data == NULL || len == 0)
  {
    return USBD_FAIL;
  }

  uint8_t result = USBD_BUSY;
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  USBD_CDC_HandleTypeDef *hcdc = hUsbDeviceFS.pClassData;
  if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED && hcdc != NULL)
  {
    if (len > tx_size)
    {
      result = USBD_FAIL;
    }
    else if (hcdc->TxState == 0)
    {
      memcpy(tx_data, data, len);
      result = CDC_Transmit_FS(tx_data, len);
    }
  }
  __set_PRIMASK(primask);
  return result;
}

uint8_t bsp_usb_host_online(void)
{
  return hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED &&
         ((host_lines & 1) || (rx_seen && (uint32_t)(HAL_GetTick() - last_rx) < 5000));
}

uint8_t bsp_usb_host_dtr(void)
{
  return host_lines & 1;
}

uint8_t bsp_usb_host_rts(void)
{
  return (host_lines >> 1) & 1;
}

