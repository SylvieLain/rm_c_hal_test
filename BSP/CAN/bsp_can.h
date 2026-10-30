#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "can.h"

/*
 * 大疆 RoboMaster 开发板 C 型 CAN 总线驱动（STM32F407IGHx + TJA1044）
 *
 * 硬件与 CubeMX 已有配置（Hardware/cubemx.ioc，MX_CAN1_Init/MX_CAN2_Init 已生成）：
 *   CAN1：PD0 = RX，PD1 = TX，2-pin 接口
 *   CAN2：PB5 = RX，PB6 = TX，4-pin 接口
 *   APB1 = 42 MHz，Prescaler = 3，BS1 = 10TQ，BS2 = 3TQ，SJW = 1TQ → 1 Mbps
 *   接口最大支持 1 Mbps，可直连 RoboMaster 电调（C620/C610 等）
 *
 * 本驱动补上 CubeMX 未生成的部分：滤波器、HAL_CAN_Start、RX0 中断、收发 API。
 * 典型用法：
 *   MX_CAN1_Init();                    // CubeMX 生成，main() 里已调用
 *   bsp_can_init(&hcan1);              // 本驱动：滤波器 + 启动 + 开中断
 *   bsp_can_set_rx_callback(can_rx);   // 可选，解电机反馈
 *   bsp_can_send_std(&hcan1, 0x200, tx_data, 8);
 *
 * 注意：
 *   - 接收回调在 CANx_RX0 中断上下文执行，必须短小，禁止阻塞、打印、HAL_Delay。
 *   - 默认滤波器接收全部标准帧和扩展帧到 FIFO0；需要精确过滤时改 bsp_can_init()。
 *   - 本文件自带 CAN1_RX0_IRQHandler/CAN2_RX0_IRQHandler，并在初始化时打开 NVIC。
 *     若以后在 CubeMX 里启用 CAN 中断并由 CubeMX 生成同名函数，请删除本文件中的实现。
 *   - 发送返回 HAL_OK 只表示数据已装入发送邮箱，不代表总线已成功发出。
 */

/* 单帧接收回调：header 含标准/扩展 ID，data 固定 8 字节缓冲（按 DLC 取有效字节） */
typedef void (*bsp_can_rx_callback_t)(CAN_HandleTypeDef *hcan, const CAN_RxHeaderTypeDef *header,
                                      const uint8_t data[8]);

/* 收发计数，用于上板排查（中断里累加） */
typedef struct
{
    uint32_t tx_frames;
    uint32_t rx_frames;
} bsp_can_stats_t;

/* 配置滤波器（接收全部帧到 FIFO0）、启动 CAN、打开 RX0 中断和 NVIC */
HAL_StatusTypeDef bsp_can_init(CAN_HandleTypeDef *hcan);

/* 注册接收回调；传 NULL 取消。回调在中断上下文执行 */
void bsp_can_set_rx_callback(bsp_can_rx_callback_t callback);

/* 发送标准数据帧，std_id ≤ 0x7FF，len ≤ 8；无空闲邮箱返回 HAL_BUSY */
HAL_StatusTypeDef bsp_can_send_std(CAN_HandleTypeDef *hcan, uint32_t std_id, const uint8_t *data, uint8_t len);

/* 发送扩展数据帧，ext_id ≤ 0x1FFFFFFF，len ≤ 8 */
HAL_StatusTypeDef bsp_can_send_ext(CAN_HandleTypeDef *hcan, uint32_t ext_id, const uint8_t *data, uint8_t len);

/* 读取收发计数 */
void bsp_can_get_stats(CAN_HandleTypeDef *hcan, bsp_can_stats_t *stats);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAN_H */
