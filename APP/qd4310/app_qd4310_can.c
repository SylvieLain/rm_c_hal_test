#include "app_qd4310_can.h"
#include "bsp_can.h"

QD4310_CAN_t qd4310 = {.id = QD4310_ID, .hcan = &hcan1};

static void qd4310_rx(CAN_HandleTypeDef *hcan, const CAN_RxHeaderTypeDef *header,
                      const uint8_t data[8])
{
    if (hcan == qd4310.hcan && header->IDE == CAN_ID_STD &&
        header->RTR == CAN_RTR_DATA && header->StdId == 0x500U + qd4310.id &&
        header->DLC == 8U)
    {
        QD4310_CAN_Update(&qd4310, data);
    }
}

HAL_StatusTypeDef qd4310_can1_init(void)
{
    bsp_can_set_rx_callback(qd4310_rx);
    return bsp_can_init(qd4310.hcan);
}
