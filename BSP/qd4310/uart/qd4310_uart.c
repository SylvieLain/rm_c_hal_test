/**
 * @file        qd4310_uart.c
 * @brief       基于HAL库的QD4310电机UART控制库
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

#include "qd4310_uart.h"

static uint8_t CRC8(const uint8_t *data, uint32_t len);

// 限制函数，用于替代C++的std::clamp
static float QD4310_UART_Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// 发送命令到电机
HAL_StatusTypeDef QD4310_UART_SendCommand(QD4310_UART_t *motor, QD4310_UART_Command_t cmd, int16_t value) {
    if (motor == NULL || motor->huart == NULL) return HAL_ERROR;

    uint8_t TxBuffer[5];
    uint8_t RxBuffer[10];
    TxBuffer[0] = motor->id;
    TxBuffer[1] = (uint8_t)cmd;
    TxBuffer[2] = (uint8_t)((uint16_t)value & 0xFF);
    TxBuffer[3] = (uint8_t)((uint16_t)value >> 8);
    TxBuffer[4] = CRC8(TxBuffer, 4);

    HAL_StatusTypeDef status = HAL_UART_Transmit(motor->huart, TxBuffer, sizeof(TxBuffer), 5);
    if (status != HAL_OK) return status;
    status = HAL_UART_Receive(motor->huart, RxBuffer, sizeof(RxBuffer), 5);
    if (status != HAL_OK) return status;
    if (RxBuffer[0] != motor->id || CRC8(RxBuffer, 9) != RxBuffer[9]) return HAL_ERROR;

    QD4310_UART_Update(motor, RxBuffer + 1);
    return HAL_OK;
}


// 更新电机状态
void QD4310_UART_Update(QD4310_UART_t *motor, const uint8_t feedback[8]) {
    motor->enabled = feedback[0] & 0x01;

    // 重构int16_t值从字节数组
    int16_t current_raw = (int16_t)((feedback[3] << 8) | feedback[2]);
    motor->current = (float)current_raw * 10.0f / INT16_MAX;

    int16_t speed_raw = (int16_t)((feedback[5] << 8) | feedback[4]);
    motor->speed = (float)speed_raw * 1000.0f / 32767.0f;

    uint16_t angle_raw = (uint16_t)((feedback[7] << 8) | feedback[6]);
    motor->angle = (float)angle_raw * QD4310_UART_TWO_PI / UINT16_MAX;
}


// 使能电机
HAL_StatusTypeDef QD4310_UART_Enable(QD4310_UART_t *motor) {
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_ENABLE, 0x0000);
}

// 失能电机
HAL_StatusTypeDef QD4310_UART_Disable(QD4310_UART_t *motor) {
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_DISABLE, 0x0000);
}

// 重启电机
HAL_StatusTypeDef QD4310_UART_Reboot(QD4310_UART_t *motor) {
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_REBOOT, 0x0000);
}

// 设置电机零点
HAL_StatusTypeDef QD4310_UART_SetZeroPos(QD4310_UART_t *motor) {
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_SET_ZERO_POS, 0x0000);
}

// 设置电机角度
HAL_StatusTypeDef QD4310_UART_SetAngle(QD4310_UART_t *motor, float angle) {
    angle = QD4310_UART_Clamp(angle, 0.0f, QD4310_UART_TWO_PI);
    int16_t angle_value = (int16_t)(uint16_t)(angle / QD4310_UART_TWO_PI * UINT16_MAX);
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_ANGLE, angle_value);
}

// 设置电机步进角度
HAL_StatusTypeDef QD4310_UART_SetStepAngle(QD4310_UART_t *motor, float step_angle) {
    step_angle = QD4310_UART_Clamp(step_angle, QD4310_UART_MIN_STEPANGLE, QD4310_UART_MAX_STEPANGLE);
    int16_t step_angle_value = (int16_t)(step_angle / QD4310_UART_MAX_STEPANGLE * INT16_MAX);
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_STEP_ANGLE, step_angle_value);
}

// 设置电机转速
HAL_StatusTypeDef QD4310_UART_SetSpeed(QD4310_UART_t *motor, float speed) {
    speed = QD4310_UART_Clamp(speed, QD4310_UART_MIN_SPEED, QD4310_UART_MAX_SPEED);
    int16_t speed_value = (int16_t)(speed / QD4310_UART_MAX_SPEED * INT16_MAX);
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_SPEED, speed_value);
}

// 设置电机低速
HAL_StatusTypeDef QD4310_UART_SetLowSpeed(QD4310_UART_t *motor, float speed) {
    speed = QD4310_UART_Clamp(speed, QD4310_UART_MIN_SPEED, QD4310_UART_MAX_SPEED);
    int16_t speed_value = (int16_t)(speed / QD4310_UART_MAX_SPEED * INT16_MAX);
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_LOW_SPEED, speed_value);
}

// 设置电机电流
HAL_StatusTypeDef QD4310_UART_SetCurrent(QD4310_UART_t *motor, float current) {
    current = QD4310_UART_Clamp(current, QD4310_UART_MIN_CURRENT, QD4310_UART_MAX_CURRENT);
    int16_t current_value = (int16_t)(current / QD4310_UART_MAX_CURRENT * INT16_MAX);
    return QD4310_UART_SendCommand(motor, QD4310_UART_CMD_CURRENT, current_value);
}

// CRC-8，多项式 0x07，初值 0，不反转
static uint8_t CRC8(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (uint8_t)((crc & 0x80) ? (crc << 1) ^ 0x07 : crc << 1);
        }
    }
    return crc;
}
