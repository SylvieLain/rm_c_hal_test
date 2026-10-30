# BSP/CAN — 大疆 RoboMaster C 板 CAN 总线驱动

`bsp_can.c/h` 是面向 RoboMaster 开发板 C 型（STM32F407IGHx + TJA1044）的 CAN 总线驱动，
补齐 CubeMX 生成代码里没有的部分：**滤波器、HAL_CAN_Start、RX0 中断、收发 API**。
CubeMX 生成的 `Hardware/Core/Src/can.c`、`can.h` 保持不动。

## 一、硬件与配置

| 项 | CAN1 | CAN2 |
|---|---|---|
| 板载接口 | 2-pin | 4-pin |
| RX | PD0（AF9） | PB5（AF9） |
| TX | PD1（AF9） | PB6（AF9） |

- 收发器：TJA1044，C 板手册标注 CAN 总线最高 1 Mbps。
- CubeMX 已配置：APB1 = 42 MHz，Prescaler = 3，BS1 = 10TQ，BS2 = 3TQ，SJW = 1TQ
  → 1 Mbps，即 RoboMaster 电调（C620/C610、GM6020）的常用速率。
- 线序：2-pin 线 A = CANL、B = CANH；4-pin 线 A = CANL、B = CANH、C = GND、D = 5V。
- 终端电阻：手册未标注板载 120 Ω，按总线拓扑在**两端**各并 120 Ω；两台设备对测时至少一端接。
- 若把 CAN1/CAN2 接在同一总线上做自测，注意两块板/两个节点都需要终端电阻，并共地。

## 二、接入方法

`main()` 里已经由 CubeMX 调用了 `MX_CAN1_Init()` / `MX_CAN2_Init()`（只做外设初始化），
在 APP 或 BSP 的板级初始化函数里再调用本驱动：

```c
#include "bsp_can.h"

void board_init(void)
{
    if (bsp_can_init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }
    bsp_can_set_rx_callback(can_rx_callback);
}
```

> 工程约定：不要在 `app_main.c` 主循环里放初始化/测试代码；初始化放到 `APP` 的初始化函数中。
> EIDE 已把 `BSP/CAN` 加入头文件路径（`.eide/eide.yml` 的 `incList`），源文件由 `srcDirs: BSP`
> 递归扫描，F8 编译会自动加入 `bsp_can.c`。

## 三、API

| 函数 | 说明 |
|---|---|
| `bsp_can_init(hcan)` | 配置滤波器（默认全接收 → FIFO0）、启动 CAN、打开 RX0 中断和 NVIC；每个 CAN 口调用一次 |
| `bsp_can_send_std(hcan, id, data, len)` | 发标准数据帧，`id ≤ 0x7FF`、`len ≤ 8`；无空闲邮箱返回 `HAL_BUSY` |
| `bsp_can_send_ext(hcan, id, data, len)` | 发扩展数据帧 |
| `bsp_can_set_rx_callback(cb)` | 注册接收回调（中断上下文执行）；`NULL` 取消 |
| `bsp_can_get_stats(hcan, &stats)` | 读 `tx_frames` / `rx_frames`，用于上板排查 |

返回值注意：`bsp_can_send_*` 返回 `HAL_OK` 只表示**已装入发送邮箱**，不代表对端已收到/ACK。

## 四、收发示例

发送（以 C620/C610 电流帧为例，ID 1~4 的控制帧是 0x200）：

```c
static void send_motor_current(int16_t i1, int16_t i2, int16_t i3, int16_t i4)
{
    uint8_t tx[8];
    tx[0] = (uint8_t)(i1 >> 8); tx[1] = (uint8_t)i1;
    tx[2] = (uint8_t)(i2 >> 8); tx[3] = (uint8_t)i2;
    tx[4] = (uint8_t)(i3 >> 8); tx[5] = (uint8_t)i3;
    tx[6] = (uint8_t)(i4 >> 8); tx[7] = (uint8_t)i4;
    (void)bsp_can_send_std(&hcan1, 0x200, tx, 8);
}
```

接收（中断里只做取帧和解码，不打印、不阻塞）：

```c
static volatile int16_t g_rpm[8];

static void can_rx_callback(CAN_HandleTypeDef *hcan, const CAN_RxHeaderTypeDef *header,
                            const uint8_t data[8])
{
    if (header->IDE != CAN_ID_STD)
    {
        return;
    }
    /* C620/C610 反馈帧：0x201 ~ 0x208，data[2..3] 为转子机械转速 */
    if (header->StdId >= 0x201U && header->StdId <= 0x208U)
    {
        g_rpm[header->StdId - 0x201U] = (int16_t)((uint16_t)data[2] << 8 | data[3]);
    }
}
```

`g_rpm` 被中断写、主循环读，必须用 `volatile`（或临界区保护）。

## 五、实现要点与限制

- **滤波器**：默认 32 位掩码全 0，标准帧和扩展帧全部进 FIFO0；需要只收某些 ID 时改
  `bsp_can_init()` 里的 `FilterIdHigh/Low` 与 `FilterMask*`，或再加一个滤波 bank。
- **滤波 bank 划分**：CAN1 用 0~13、CAN2 用 14~27（`SlaveStartFilterBank = 14`），
  驱动已处理 CAN2 的 bank 偏移，单独用 CAN2 也能正常接收。
- **中断归属**：`.ioc` 目前没有启用 CAN NVIC，所以 `CAN1_RX0_IRQHandler` /
  `CAN2_RX0_IRQHandler` 由 `bsp_can.c` 自带。若以后在 CubeMX 里勾选 CAN 中断并由 CubeMX
  生成同名函数，请删除 `bsp_can.c` 里的实现，避免重复定义。
- **总线异常恢复**：CubeMX 当前把 `AutoBusOff` 和 `AutoRetransmission` 都设为 DISABLE。
  接线错误/无终端电阻/无 ACK 时会累积错误直到 bus-off 且不自动恢复，需要复位板子。
  正式上车建议在 CubeMX 中打开 AUTO BusOff 和自动重传后重新生成（本驱动不用改）。
- **验证层级**：目前只做到 EIDE 全量编译 + 链接通过（`bsp_can.o` 已编译，符号已进固件），
  **没有上板验证**。上板建议顺序：
  1. 两块 C 板 CANH-CANH / CANL-CANL / GND 共地、两端 120 Ω，一块每秒发一帧，另一块看
     `rx_frames` 增长；
  2. 再按上面的 C620 示例接单个电机（先小电流/低速），确认 0x201 反馈解码正常；
  3. 最后接入 APP 控制逻辑。
