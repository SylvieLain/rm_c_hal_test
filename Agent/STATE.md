/*!!不要修改该内部内容!!*/
# STATE - 当前工程状态
 - 每次修改需要 清除保护内容外后重新写
 (例如: ## 当前任务：直立 PD 环标定完成，待松手长稳验证
        ## 状态：零点 -125、kp=2560、kd=1024 已固化烧录（SHA 58f00bdf），扶车闭环 ±2.4° 稳定
        ## 缺点:抗干扰较差)
/*!!不要修改该内部内容!!*/

## 当前任务：板载驱动完成，实机验收通过；业务逻辑已按要求移除，只留驱动。
## 板载驱动：BSP/Board/board.c/h。board_init 启动 TIM5/TIM4 PWM，静默输出（灯灭、蜂鸣停）。board_key_read 20 ms 消抖；set_board_led 按颜色常量设 RGB（PH12/PH11/PH10 TIM5 PWM 共阳极低电平点亮）；board_beep(freq, ms)/board_buzzer_stop 非阻塞蜂鸣（PD14 TIM4_CH3），定时停止挂在 HAL_SYSTICK_Callback（1 ms），业务层无需轮询。无按键业务、无上电亮灯/鸣叫。
## APP：user_main 调 qd4310_can1_init 失败进 Error_Handler，随后 board_init，主循环为空。短按/长按等按键语义留给使用方自定义。QD4310_ID=0，直接暴露 qd4310 对象；qd4310_can1_init 复用 BSP/CAN 启动 CAN1，并过滤 0x500+ID 的标准数据帧（DLC 8）更新电机状态。
## 配置：CubeMX 固定外设、引脚、DMA 和 CAN 位时序未改；EIDE 增加 BSP/Board 头文件路径。
## 实测：2026-10-31，按键/RGB 灯/蜂鸣器业务版本实机验收通过（短按切色、长按关灯、蜂鸣正常），随后移除业务；CAN1、1 Mbps、ID 0，接线 H-H/L-L/共地，反馈正常（ESR/ErrorCode/电机错误码均 0），10/20 rpm 稳定段末 10 次均速 9.952/20.072 rpm，结束转速 0、使能位 0。
## 验证：EIDE 全量编译通过（保留原 BMI088 未使用变量警告）；板载业务版本实机通过，驱动版本编译验证，行为与业务版一致（去掉了业务调用）。
## 限制：CAN 总线目前只有一个接收回调槽；后续接其他 CAN 设备时统一分发。USB 环形队列、其他外设未测试。
