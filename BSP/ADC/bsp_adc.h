#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "struct_typedef.h"

/* 电压检测驱动（源：DJI 官方 C 板例程 7.ADC_24V_power/bsp/boards/bsp_adc.c）
   PF10 = ADC3_IN8 = ADC_BAT，电池经 22K/200K 分压，比例 (22K+200K)/22K ≈ 10.09 */
extern void init_vrefint_reciprocal(void);
extern fp32 get_temprate(void);
extern fp32 get_battery_voltage(void);
extern uint8_t get_hardware_version(void);
#endif
