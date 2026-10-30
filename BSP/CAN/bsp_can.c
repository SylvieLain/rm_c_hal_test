/**
  ******************************************************************************
  * @file    bsp_can.c
  * @brief   大疆 RoboMaster C 板 CAN 总线驱动（HAL，无 RTOS）
  * @details CAN1/CAN2 滤波器配置、启动、RX0 中断接收、标准/扩展帧发送。
  *          CubeMX 已把两路 CAN 配成 1 Mbps（APB1 42 MHz / 3 / 10TQ / 3TQ）。
  ******************************************************************************
  */
#include "bsp_can.h"

/* CAN1 使用滤波 bank 0~13，CAN2 使用 14~27（CAN2SB 默认 14） */
#define BSP_CAN_FILTER_BANK_CAN1    0U
#define BSP_CAN_FILTER_BANK_CAN2    14U
#define BSP_CAN_SLAVE_START_BANK    14U

/* 与工程里 USB 中断保持一致的中断优先级（NVIC_PRIORITYGROUP_4，越小越高） */
#define BSP_CAN_IRQ_PRIORITY        5U

static bsp_can_rx_callback_t s_rx_callback = NULL;
static bsp_can_stats_t s_stats[2]; /* [0] = CAN1，[1] = CAN2 */

static uint8_t bsp_can_index(CAN_HandleTypeDef *hcan)
{
    return (hcan->Instance == CAN2) ? 1U : 0U;
}

HAL_StatusTypeDef bsp_can_init(CAN_HandleTypeDef *hcan)
{
    CAN_FilterTypeDef filter = {0};
    HAL_StatusTypeDef status;

    if (hcan == NULL || hcan->Instance == NULL)
    {
        return HAL_ERROR;
    }

    /* 32 位掩码模式，掩码全 0：标准帧和扩展帧全部进入 FIFO0 */
    filter.FilterBank = (hcan->Instance == CAN2) ? BSP_CAN_FILTER_BANK_CAN2 : BSP_CAN_FILTER_BANK_CAN1;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0000U;
    filter.FilterIdLow = 0x0000U;
    filter.FilterMaskIdHigh = 0x0000U;
    filter.FilterMaskIdLow = 0x0000U;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation = CAN_FILTER_ENABLE;
    filter.SlaveStartFilterBank = BSP_CAN_SLAVE_START_BANK;

    status = HAL_CAN_ConfigFilter(hcan, &filter);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_CAN_Start(hcan);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    if (status != HAL_OK)
    {
        return status;
    }

    if (hcan->Instance == CAN1)
    {
        HAL_NVIC_SetPriority(CAN1_RX0_IRQn, BSP_CAN_IRQ_PRIORITY, 0U);
        HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    }
    else
    {
        HAL_NVIC_SetPriority(CAN2_RX0_IRQn, BSP_CAN_IRQ_PRIORITY, 0U);
        HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    }

    s_stats[bsp_can_index(hcan)].tx_frames = 0U;
    s_stats[bsp_can_index(hcan)].rx_frames = 0U;

    return HAL_OK;
}

void bsp_can_set_rx_callback(bsp_can_rx_callback_t callback)
{
    s_rx_callback = callback;
}

HAL_StatusTypeDef bsp_can_send_std(CAN_HandleTypeDef *hcan, uint32_t std_id, const uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef header = {0};
    uint32_t mailbox = 0U;
    HAL_StatusTypeDef status;

    if (hcan == NULL || hcan->Instance == NULL || data == NULL || len > 8U || std_id > 0x7FFU)
    {
        return HAL_ERROR;
    }

    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0U)
    {
        return HAL_BUSY;
    }

    header.StdId = std_id;
    header.ExtId = 0U;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = len;
    header.TransmitGlobalTime = DISABLE;

    status = HAL_CAN_AddTxMessage(hcan, &header, data, &mailbox);
    if (status == HAL_OK)
    {
        s_stats[bsp_can_index(hcan)].tx_frames++;
    }

    return status;
}

HAL_StatusTypeDef bsp_can_send_ext(CAN_HandleTypeDef *hcan, uint32_t ext_id, const uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef header = {0};
    uint32_t mailbox = 0U;
    HAL_StatusTypeDef status;

    if (hcan == NULL || hcan->Instance == NULL || data == NULL || len > 8U || ext_id > 0x1FFFFFFFU)
    {
        return HAL_ERROR;
    }

    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0U)
    {
        return HAL_BUSY;
    }

    header.ExtId = ext_id;
    header.StdId = 0U;
    header.IDE = CAN_ID_EXT;
    header.RTR = CAN_RTR_DATA;
    header.DLC = len;
    header.TransmitGlobalTime = DISABLE;

    status = HAL_CAN_AddTxMessage(hcan, &header, data, &mailbox);
    if (status == HAL_OK)
    {
        s_stats[bsp_can_index(hcan)].tx_frames++;
    }

    return status;
}

void bsp_can_get_stats(CAN_HandleTypeDef *hcan, bsp_can_stats_t *stats)
{
    if (hcan == NULL || hcan->Instance == NULL || stats == NULL)
    {
        return;
    }

    *stats = s_stats[bsp_can_index(hcan)];
}

/* HAL 弱函数重载：FIFO0 有待处理帧时被 HAL_CAN_IRQHandler 调用 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef header;
    uint8_t data[8];
    uint8_t index = bsp_can_index(hcan);

    /* 一次中断可能积压 1~3 帧，全部取出，避免 FIFO 溢出丢帧 */
    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0U)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, data) != HAL_OK)
        {
            break;
        }

        s_stats[index].rx_frames++;

        if (s_rx_callback != NULL)
        {
            s_rx_callback(hcan, &header, data);
        }
    }
}

/* CAN1 RX0 中断入口（.ioc 未启用 CAN NVIC，故由驱动自带） */
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

/* CAN2 RX0 中断入口 */
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan2);
}
