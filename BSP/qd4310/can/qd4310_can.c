/**
 * @file        qd4310_can.c
 * @brief       基于HAL库的QD4310电机CAN总线控制库
 * @details
 * @author      Liu-Curiousity (2675794963@qq.com)
 * @date        2026-6-15
 * @version     V1.2.0
 * @note
 * @warning
 * @par         历史版本:
 *		        V1.1.0创建于2026-1-25
 *		        V1.2.0创建于2026-6-15, 添加零点设置和重启设备
 * @copyright   (c) 2026 QDrive
 */

#include "qd4310_can.h"

// 限制函数，用于替代C++的std::clamp
static float QD4310_CAN_Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// 发送命令到电机
HAL_StatusTypeDef QD4310_CAN_SendCommand(QD4310_CAN_t *motor, QD4310_CAN_Command_t cmd, int16_t value) {
    if (motor == NULL || motor->hcan == NULL) return HAL_ERROR;
    if (HAL_CAN_GetTxMailboxesFreeLevel(motor->hcan) == 0) return HAL_BUSY;

    uint8_t TxBuffer[3];
    TxBuffer[0] = (uint8_t)cmd;
    TxBuffer[1] = (uint8_t)((uint16_t)value & 0xFF);
    TxBuffer[2] = (uint8_t)((uint16_t)value >> 8);

    uint32_t txMailbox;
    CAN_TxHeaderTypeDef TxHeader = {0};
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.StdId = 0x400 + motor->id;
    TxHeader.TransmitGlobalTime = DISABLE;
    TxHeader.DLC = 3;
    return HAL_CAN_AddTxMessage(motor->hcan, &TxHeader, TxBuffer, &txMailbox);
}


// 更新电机状态
void QD4310_CAN_Update(QD4310_CAN_t *motor, const uint8_t feedback[8]) {
    motor->enabled = feedback[0] & 0x01;

    // 重构int16_t值从字节数组
    int16_t current_raw = (int16_t)((feedback[3] << 8) | feedback[2]);
    motor->current = (float)current_raw * 10.0f / INT16_MAX;

    int16_t speed_raw = (int16_t)((feedback[5] << 8) | feedback[4]);
    motor->speed = (float)speed_raw * 1000.0f / 32767.0f;

    uint16_t angle_raw = (uint16_t)((feedback[7] << 8) | feedback[6]);
    motor->angle = (float)angle_raw * QD4310_CAN_TWO_PI / UINT16_MAX;
}


// 使能电机
HAL_StatusTypeDef QD4310_CAN_Enable(QD4310_CAN_t *motor) {
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_ENABLE, 0x0000);
}

// 失能电机
HAL_StatusTypeDef QD4310_CAN_Disable(QD4310_CAN_t *motor) {
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_DISABLE, 0x0000);
}

// 重启电机
HAL_StatusTypeDef QD4310_CAN_Reboot(QD4310_CAN_t *motor) {
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_REBOOT, 0x0000);
}

// 设置电机零点
HAL_StatusTypeDef QD4310_CAN_SetZeroPos(QD4310_CAN_t *motor) {
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_SET_ZERO_POS, 0x0000);
}

// 设置电机角度
HAL_StatusTypeDef QD4310_CAN_SetAngle(QD4310_CAN_t *motor, float angle) {
    angle = QD4310_CAN_Clamp(angle, 0.0f, QD4310_CAN_TWO_PI);
    int16_t angle_value = (int16_t)(uint16_t)(angle / QD4310_CAN_TWO_PI * UINT16_MAX);
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_ANGLE, angle_value);
}

// 设置电机步进角度
HAL_StatusTypeDef QD4310_CAN_SetStepAngle(QD4310_CAN_t *motor, float step_angle) {
    step_angle = QD4310_CAN_Clamp(step_angle, QD4310_CAN_MIN_STEPANGLE, QD4310_CAN_MAX_STEPANGLE);
    int16_t step_angle_value = (int16_t)(step_angle / QD4310_CAN_MAX_STEPANGLE * INT16_MAX);
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_STEP_ANGLE, step_angle_value);
}

// 设置电机转速
HAL_StatusTypeDef QD4310_CAN_SetSpeed(QD4310_CAN_t *motor, float speed) {
    speed = QD4310_CAN_Clamp(speed, QD4310_CAN_MIN_SPEED, QD4310_CAN_MAX_SPEED);
    int16_t speed_value = (int16_t)(speed / QD4310_CAN_MAX_SPEED * INT16_MAX);
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_SPEED, speed_value);
}

// 设置电机低速
HAL_StatusTypeDef QD4310_CAN_SetLowSpeed(QD4310_CAN_t *motor, float speed) {
    speed = QD4310_CAN_Clamp(speed, QD4310_CAN_MIN_SPEED, QD4310_CAN_MAX_SPEED);
    int16_t speed_value = (int16_t)(speed / QD4310_CAN_MAX_SPEED * INT16_MAX);
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_LOW_SPEED, speed_value);
}

// 设置电机电流
HAL_StatusTypeDef QD4310_CAN_SetCurrent(QD4310_CAN_t *motor, float current) {
    current = QD4310_CAN_Clamp(current, QD4310_CAN_MIN_CURRENT, QD4310_CAN_MAX_CURRENT);
    int16_t current_value = (int16_t)(current / QD4310_CAN_MAX_CURRENT * INT16_MAX);
    return QD4310_CAN_SendCommand(motor, QD4310_CAN_CMD_CURRENT, current_value);
}
