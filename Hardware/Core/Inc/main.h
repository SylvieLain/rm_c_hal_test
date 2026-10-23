/**
  ******************************************************************************
  * @file    main.h
  * @brief   RoboMaster C 型开发板 (STM32F407IGHx) 板级引脚定义
  ******************************************************************************
  * 引脚来源: 20.standard_robot/standard_robot.ioc (权威配置)
  ******************************************************************************
  */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

void Error_Handler(void);

/* ---------------- LED (TIM5 CH1/2/3, 低电平点亮) ---------------- */
#define LED_R_Pin            GPIO_PIN_12
#define LED_R_GPIO_Port      GPIOH
#define LED_G_Pin            GPIO_PIN_11
#define LED_G_GPIO_Port      GPIOH
#define LED_B_Pin            GPIO_PIN_10
#define LED_B_GPIO_Port      GPIOH

/* ---------------- 蜂鸣器 (TIM4_CH3) ---------------- */
#define BUZZER_Pin           GPIO_PIN_14
#define BUZZER_GPIO_Port     GPIOD

/* ---------------- 按键 / 拨码 ---------------- */
#define KEY_Pin              GPIO_PIN_0
#define KEY_GPIO_Port        GPIOA

#define HW0_Pin              GPIO_PIN_0
#define HW0_GPIO_Port        GPIOC
#define HW1_Pin              GPIO_PIN_1
#define HW1_GPIO_Port        GPIOC
#define HW2_Pin              GPIO_PIN_2
#define HW2_GPIO_Port        GPIOC

#define BUTTON_TRIG_Pin      GPIO_PIN_7
#define BUTTON_TRIG_GPIO_Port GPIOI

/* ---------------- BMI088 SPI 片选 / IST8310 ---------------- */
#define CS1_ACCEL_Pin        GPIO_PIN_4
#define CS1_ACCEL_GPIO_Port  GPIOA
#define CS1_GYRO_Pin         GPIO_PIN_0
#define CS1_GYRO_GPIO_Port   GPIOB
#define SPI2_CS_Pin          GPIO_PIN_12
#define SPI2_CS_GPIO_Port    GPIOB
#define RSTN_IST8310_Pin     GPIO_PIN_6
#define RSTN_IST8310_GPIO_Port GPIOG

/* ---------------- 电池电压 (ADC3_IN8) ---------------- */
#define ADC_BAT_Pin          GPIO_PIN_10
#define ADC_BAT_GPIO_Port    GPIOF

/* ---------------- 串口 ---------------- */
/* USART1: TX PA9  / RX PB7  (AF7)  —— 原工程 printf 口 */
/* USART2: TX PD5  / RX PD6  (AF7)  —— C 板“串口2”，接 CH340 */
/* USART3: TX PC10 / RX PC11 (AF7)  —— 原工程 DBUS, 100000 8E1 */
/* USART6: TX PG14 / RX PG9  (AF8)  —— C 板“串口4” */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
