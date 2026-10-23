/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   异常与 SysTick 中断（无 RTOS，HAL 时基）
  ******************************************************************************
  */
#include "main.h"
#include "stm32f4xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* 出错时点亮红灯，方便无调试器时肉眼定位 */
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
  while (1)
  {
  }
}

void MemManage_Handler(void) { while (1) { } }
void BusFault_Handler(void)  { while (1) { } }
void UsageFault_Handler(void){ while (1) { } }
void SVC_Handler(void)       { }
void DebugMon_Handler(void)  { }
void PendSV_Handler(void)    { }

void SysTick_Handler(void)
{
  HAL_IncTick();
}
