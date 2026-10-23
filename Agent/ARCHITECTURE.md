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

# BMI088（SPI1）
| 信号 | 引脚 |
|---|---|
| SCK | PB3 (AF5) |
| MISO | PB4 (AF5) |
| MOSI | PA7 (AF5) |
| CS_ACCEL | PA4 |
| CS_GYRO | PB0 |

Mode 3、8 bit、NSS 软件、`84 MHz / 256 = 328 kHz`（与原工程一致）。
自检项：ACC 芯片 ID `0x1E`、GYRO 芯片 ID `0x0F`、`BMI088_init()` 自检返回值、5 Hz 加速度/角速度/温度。
打印用整数（mg / 0.01 dps / 0.1 ℃），不依赖浮点 printf。

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

