/*!!不要修改该内部内容!!*/
# STATE - 当前工程状态
 - 每次修改需要 清除保护内容外后重新写
 (例如: ## 当前任务：直立 PD 环标定完成，待松手长稳验证
        ## 状态：零点 -125、kp=2560、kd=1024 已固化烧录（SHA 58f00bdf），扶车闭环 ±2.4° 稳定
        ## 缺点:抗干扰较差)
/*!!不要修改该内部内容!!*/

## 当前任务：PF10 电池电压驱动加入并实测通过（11.95 V 左右，期望约 12.0 V）；另有 BSP/PWM 七路舵机 PWM 已完成。
## 电压驱动：BSP/ADC/bsp_adc.c/h，源自 DJI 官方 C 板例程（RoboMaster/Development-Board-C-Examples 7.ADC_24V_power）。get_battery_voltage() 读 ADC3_IN8（PF10/ADC_BAT），按 (22K+200K)/22K=10.09 分压比换算；init_vrefint_reciprocal() 采 200 次内部 VREFINT(1.2V) 反推 VDDA，不依赖 3.3V 精度。与官方差异：采样时间 3→480 周期（分压源阻抗约 20K）。附带 get_temprate、get_hardware_version(HW0/1/2)。
## 电压实测：2026-10-31，ST-Link 读 RAM（临时固件 16 次均值存 vbat_mv@0x20000130，bin/vbat_test_main.c 在 gitignore 的 /bin 内），HotPlug 模式三次读数 11.960/11.968/11.948 V 稳定；builder.params 已恢复 app_main.c，正式固件已重新烧录运行。
## PWM：BSP/PWM/bsp_pwm.c/h。C1~C4 为 TIM1 CH1~4（PE9/PE11/PE13/PE14），C5~C7 为 TIM8 CH1~3（PC6/PI6/PI7）；B 排 5V，A 排 GND。50 Hz、1 us 计数步长，初始化启动七路脉宽清零；按通道设 us 脉宽/0~1000 占空比/启停，返回 HAL 状态。未自动接入 APP 或 board_init。
## CubeMX：Hardware/cubemx.ioc 将 PI7 从旧 BUTTON_TRIG 输入改为 TIM8_CH3；工程外生成后仅同步 tim.c、gpio.c、main.h。旧 BUTTON_TRIG 宏移除，其余外设配置与中断不变。EIDE 加入 BSP/PWM、BSP/ADC 路径，builder.params 已含源文件。
## 板载/APP：board 驱动（key/RGB/蜂鸣器）保留；RGB 为 PWM1 高电平点亮，关闭 CCR=0，全亮 CCR=ARR+1。user_main：qd4310_can1_init 失败进 Error_Handler，board_init 后主循环为空。QD4310_ID=0，CAN1 过滤 0x500+ID 标准数据帧（DLC 8）。
## 验证：EIDE 全量编译链接通过；电压驱动编译+ST-Link 实测通过；PWM 工程外 MSVC 测试通过（七路映射、零脉宽启动、0/100% 边界、越界、独立停止、HAL 状态传递），板上波形未测；CAN1 电机实测通过（1 Mbps、ID 0、H-H/L-L/共地，10/20 rpm 均速 9.952/20.072 rpm）。
## 限制：CAN 总线只有一个接收回调槽；USB 环形队列未做板上验收；其他外设未测试。
