#ifndef BSP_PWM_H
#define BSP_PWM_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 通道 1~7 对应 C1~C7；B 排 5V，A 排 GND。
   TIM1/TIM8 由 CubeMX 初始化：50 Hz，计数步长 1 us。
   C1~C4 共用 TIM1 周期，C5~C7 共用 TIM8 周期。 */
HAL_StatusTypeDef bsp_pwm_init(void);
HAL_StatusTypeDef bsp_pwm_start(uint8_t channel);
HAL_StatusTypeDef bsp_pwm_stop(uint8_t channel);

/* 高电平脉宽，单位 us，范围 0~20000（当前 CubeMX 配置）。 */
HAL_StatusTypeDef bsp_pwm_set_pulse(uint8_t channel, uint32_t pulse_us);

/* 占空比 0~1000，分别对应 0%~100%。 */
HAL_StatusTypeDef bsp_pwm_set_duty(uint8_t channel, uint16_t duty);

#ifdef __cplusplus
}
#endif

#endif
