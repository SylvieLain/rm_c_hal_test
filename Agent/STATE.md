/*!!不要修改该内部内容!!*/
# STATE - 当前工程状态
 - 每次修改需要 清除保护内容外后重新写
 (例如: ## 当前任务：直立 PD 环标定完成，待松手长稳验证
        ## 状态：零点 -125、kp=2560、kd=1024 已固化烧录（SHA 58f00bdf），扶车闭环 ±2.4° 稳定
        ## 缺点:抗干扰较差)
/*!!不要修改该内部内容!!*/

## 当前任务：板载按键/RGB 灯/蜂鸣器业务逻辑完成，待实机验收。
## 板载业务：BSP/Board/board.c/h。board_init 启动 TIM5/TIM4 PWM，上电红灯+短鸣；board_update 主循环非阻塞：短按切灯色（红→蓝→绿→紫→黄→橙→灭）并 2 kHz 短鸣，长按 800 ms 关灯并 500 Hz 低鸣。board_key_read 20 ms 消抖保留；board_beep/board_buzzer_stop 非阻塞定时蜂鸣。RGB 为 PH12(R)/PH11(G)/PH10(B) TIM5 PWM 共阳极低电平点亮，蜂鸣器 PD14 TIM4_CH3，均为 CubeMX 已有配置。
## APP：user_main 调 qd4310_can1_init 失败进 Error_Handler，随后 board_init，主循环调 board_update。QD4310_ID=0，直接暴露 qd4310 对象；qd4310_can1_init 复用 BSP/CAN 启动 CAN1，并过滤 0x500+ID 的标准数据帧（DLC 8）更新电机状态。
## 配置：CubeMX 固定外设、引脚、DMA 和 CAN 位时序未改；EIDE 增加 BSP/Board 头文件路径。
## 实测（上轮）：2026-10-31，CAN1、1 Mbps、ID 0；接线 H-H/L-L/共地恢复后反馈正常，CAN ESR/ErrorCode 和电机错误码均为 0，10/20 rpm 首个稳定段末 10 次均速 9.952/20.072 rpm。结束确认转速 0、使能位 0。
## 验证：EIDE 全量编译通过（保留原 BMI088 未使用变量警告）；本轮板载按键/灯/蜂鸣器逻辑仅编译验证，未做实机验收（按键、灯色、蜂鸣时长、极性均需上板确认）。
## 限制：CAN 总线目前只有一个接收回调槽；后续接其他 CAN 设备时统一分发。USB 环形队列、其他外设未测试。
