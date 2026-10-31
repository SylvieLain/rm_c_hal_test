# C 板七路 PWM

| 接口 | 信号引脚 | 定时器通道 |
|---|---|---|
| C1 | PE9 | TIM1_CH1 |
| C2 | PE11 | TIM1_CH2 |
| C3 | PE13 | TIM1_CH3 |
| C4 | PE14 | TIM1_CH4 |
| C5 | PC6 | TIM8_CH1 |
| C6 | PI6 | TIM8_CH2 |
| C7 | PI7 | TIM8_CH3 |

B1~B7 是 5V 电源，A1~A7 是 GND。PWM 信号由 MCU 输出，不能把 B 排电源当作信号电平。
映射依据：[大疆 C 板手册](https://rm-static.djicdn.com/tem/35228/RoboMaster%20Development%20Board%20Type%20C%20User%20Manual.pdf)。

CubeMX 配置：TIM1/TIM8 时钟 168 MHz，PSC=167、ARR=19999，计数步长 1 us，周期 20 ms（50 Hz）。
C1~C4 共用 TIM1 周期，C5~C7 共用 TIM8 周期；七路脉宽独立。修改频率在 `.ioc` 中调整对应定时器周期；`bsp_pwm_set_pulse` 要求保留 1 us 计数步长。

`MX_TIM1_Init()`、`MX_TIM8_Init()` 已由生成的 main 调用，之后业务层按需调用：

```c
#include "bsp_pwm.h"

bsp_pwm_init();                 // 启动七路，初始脉宽全部为 0
bsp_pwm_set_pulse(1, 1500);      // C1 高电平 1500 us
bsp_pwm_set_duty(7, 500);        // C7 占空比 50%，不是舵机中位
bsp_pwm_stop(1);                // 清除 C1 脉宽并停止该路
bsp_pwm_start(1);               // 重新启动，脉宽仍为 0
```

通道参数为 1~7；占空比参数为 0~1000，支持精确 0%/100%；脉宽不能超过当前周期。参数非法返回 `HAL_ERROR`，启停返回 HAL 状态，实际使用时检查返回值。

本模块没有自动接入 `board_init` 或 APP，也不添加中断、舵机角度换算或电调解锁业务。C7 已从旧 `BUTTON_TRIG` 输入配置改为 PWM，不能同时作为该输入使用。
