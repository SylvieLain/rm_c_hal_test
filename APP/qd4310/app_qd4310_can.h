#ifndef APP_QD4310_CAN_H
#define APP_QD4310_CAN_H

#include "qd4310_can.h"

#define QD4310_ID 0U

extern QD4310_CAN_t qd4310;

/* CAN1，1 Mbps；仅初始化总线和反馈，不使能电机。 */
HAL_StatusTypeDef qd4310_can1_init(void);

#endif
