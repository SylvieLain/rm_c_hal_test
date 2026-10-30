/*!!不要修改该内部内容!!*/
# 规则
- 此文件为长期工程项目描述文件,禁止修改如遇重要架构修改 
- 明文需要先打报告审核

# 架构与方向
  ## 架构
- `main.c` 保持薄，复杂逻辑放入 `APP`、`BSP`。
- 'APP'内容是组合业务逻辑,不要放置驱动模块和模块化逻辑
- 'BSP'内容是类unix模块化实现中间层,包含小逻辑模块和驱动层逻辑
- 'Algorithm'内容是算法代码逻辑的公式 不要放置具体业务逻辑 只负责定义变量和调用公式

  ## 方向
- 优先增量修改现有可用逻辑，不为低概率问题增加复杂状态机和保护层。
- CubeMX 生成配置以 `.ioc` 为来源；非任务需要不手改自动生成区域。
- 嵌入式流程优先非阻塞、中断驱动和清晰状态机；中断内只做必要快操作。
- 保留已有注释和命名，文件使用 UTF-8/LF，不随意重组工程。
- 编译、烧录、在线数据和物理动作分别验收，不用模型结论替代硬件证据。

/*!!不要修改该内部内容!!*/

# 工具链

- STM32CubeMX维护 `.ioc`；优先使用 EIDE 
- 编译优先按F8；命令行复现使用EIDE的 `unify_builder.exe -p build\Debug\builder.params --no-color --rebuild`。当前可执行文件位于 `C:\Users\蛟龙\.vscode\extensions\cl.eide-3.27.2\res\tools\win32\unify_builder\unify_builder.exe`。新增模块后先确认 `builder.params` 的 `sourceList` 已包含源文件。
- 烧录优先按F9，使用项目配置的ST-Link、SWD 4 MHz

# 外设状态

板子为大疆rm c板

# BMI088（SPI1）
| 信号 | 引脚 |
|---|---|
| SCK | PB3 (AF5) |
| MISO | PB4 (AF5) |
| MOSI | PA7 (AF5) |
| CS_ACCEL | PA4 |
| CS_GYRO | PB0 |

Mode 3、8 bit、NSS 软件、`84 MHz / 256 = 328 kHz`
打印用整数（mg / 0.01 dps / 0.1 ℃），不依赖浮点 printf。

# QD4310（CAN1）
依据 C 板用户手册的 CAN 接口线序、QD4310 使用手册的 CAN 协议，并与当前 `Hardware/cubemx.ioc` 位时序核对。

| 项目 | 配置 |
|---|---|
| C 板接口 | CAN1，2-pin；Pin 1 = CANL，Pin 2 = CANH |
| STM32 引脚 | CAN1_RX = PD0 (AF9)，CAN1_TX = PD1 (AF9) |
| 波特率 | 1 Mbps；APB1 = 42 MHz，Prescaler = 3，BS1 = 10 TQ，BS2 = 3 TQ，SJW = 1 TQ |
| 帧格式 | 11-bit 标准数据帧，不使用扩展帧 |
| QD4310 供电 | 7~26 V |
| 电机 CAN ID | 工程配置为 `0x00`；有效范围 `0x00`~`0x0F`，同一总线上必须唯一 |
| 控制帧 | 标准 ID `0x400 + ID`；ID 0 时为 `0x400`，DLC = 3 |
| 反馈帧 | 标准 ID `0x500 + ID`；ID 0 时为 `0x500`，DLC = 8 |
| 控制数据 | Byte 0 为指令；Byte 1~2 为控制量，低字节在前 |
| 反馈数据 | Byte 0 状态、Byte 1 错误码、Byte 2~3 电流、Byte 4~5 转速、Byte 6~7 角度；多字节值低字节在前 |

控制指令：`0x00` 请求反馈、`0x01` 使能、`0x02` 失能、`0x03` 电流、`0x04` 速度、`0x05` 角度、`0x06` 低速、`0x07` 角度步进、`0xFF` 重启、`0xFE` 设置零点、`0xFB` 清除错误。

# LED 与按键
依据 C 板用户手册；当前引脚功能以 `Hardware/cubemx.ioc` 为准。

| 外设 | 引脚/配置 | 说明 |
|---|---|---|
| 蓝色 LED | PH10，TIM5_CH1 | 用户 RGB LED；手册说明 IO 高电平点亮 |
| 绿色 LED | PH11，TIM5_CH2 | 用户 RGB LED；手册说明 IO 高电平点亮 |
| 红色 LED | PH12，TIM5_CH3 | 用户 RGB LED；手册说明 IO 高电平点亮 |
| 用户按键 KEY | PA0，GPIO 输入，上拉 | 按下时为低电平 |

# UART
C 板外壳丝印与 STM32 外设编号不一致，按手册和当前工程映射如下：

| 外壳丝印/接口 | STM32 外设 | 引脚 | 接口线序 | 当前波特率 |
|---|---|---|---|---|
| UART1（3-pin） | USART6 | TX = PG14，RX = PG9 | GND、TX、RX | 115200 bps |
| UART2（4-pin） | USART1 | TX = PA9，RX = PB7 | RXD、TXD、GND、5V | 115200 bps |

串口为 3.3 V / 5 V 电平；连接另一设备时 TX、RX 交叉连接。RS-232/RS-485 需外接电平转换器。

# PWM
依据 C 板用户手册，板载 7 路 PWM 舵机接口总输出电流上限为 5 A；接口通道为 TIM1_CH1~CH4、TIM8_CH1~CH3。

| 功能 | 当前工程映射 |
|---|---|
| TIM1_CH1~CH4 | PE9、PE11、PE13、PE14 |
| TIM8_CH1~CH2 | PC6、PI6 |
| TIM8_CH3 | 当前 `.ioc` 中未配置为 PWM；PI7 配置为 GPIO 输入 |
| 5V 接口控制 | TIM3_CH3，PC8 |
| 蜂鸣器 | TIM4_CH3，PD14 |
| RGB LED 亮度 | TIM5_CH1~CH3，PH10~PH12 |

# 时钟
HSE 12 MHz → PLLM=6 / PLLN=168 / PLLP=2 / PLLQ=7 → SYSCLK 168 MHz
AHB 168 MHz / APB1 42 MHz / APB2 84 MHz，Flash latency 5。

# 编译
eide：打开 `外设测试.code-workspace` → Build。

命令行等价参数：

```
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
  -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=12000000 \
  -IHardware/Core/Inc -IHardware/Drivers/STM32F4xx_HAL_Driver/Inc \
  -IHardware/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
  -IHardware/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IHardware/Drivers/CMSIS/Include \
  -IHardware/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
  -IHardware/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
  -IBSP -IApp -Og -g3 -ffunction-sections -fdata-sections -std=gnu11 ...
```

链接：`-THardware/STM32F407XX_FLASH.ld --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -lm`

# 烧录
（若当前无 SWD 下载线，用 USB DFU）

板子进入 DFU（BOOT0 高电平后复位/上电），然后：

```
STM32_Programmer_CLI.exe -c port=USB1 -w build\peripheral_test.hex -v
STM32_Programmer_CLI.exe -c port=USB1 -g 0x08000000

