/**
 * @file        qd4310_can.h
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

#ifndef QD4310_CAN_H
#define QD4310_CAN_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// 命令枚举
typedef enum {
    QD4310_CAN_CMD_NOP = 0x00,
    QD4310_CAN_CMD_ENABLE = 0x01,
    QD4310_CAN_CMD_DISABLE = 0x02,
    QD4310_CAN_CMD_CURRENT = 0x03,
    QD4310_CAN_CMD_SPEED = 0x04,
    QD4310_CAN_CMD_ANGLE = 0x05,
    QD4310_CAN_CMD_LOW_SPEED = 0x06,
    QD4310_CAN_CMD_STEP_ANGLE = 0x07,

    QD4310_CAN_CMD_REBOOT = 0xFF,
    QD4310_CAN_CMD_SET_ZERO_POS = 0xFE,
    QD4310_CAN_CMD_CLEAR_ERROR = 0xFB,
} QD4310_CAN_Command_t;

// QD4310电机结构体
typedef struct {
    bool enabled;
    uint8_t id;
    float speed;   // 转速，单位rpm
    float angle;   // 角度，单位弧度
    float current; // 电流，单位A
    CAN_HandleTypeDef *hcan;
} QD4310_CAN_t;

// 函数声明
/* 控制函数只提交发送邮箱；无空闲邮箱返回 HAL_BUSY，HAL_OK 不代表电机已执行。 */
/**
 * @brief 使能电机
 * @param motor 电机结构体指针
 */
HAL_StatusTypeDef QD4310_CAN_Enable(QD4310_CAN_t *motor);
/**
 * @brief 失能电机
 * @param motor 电机结构体指针
 */
HAL_StatusTypeDef QD4310_CAN_Disable(QD4310_CAN_t *motor);
/**
 * @brief 重启电机
 * @param motor 电机结构体指针
 */
HAL_StatusTypeDef QD4310_CAN_Reboot(QD4310_CAN_t *motor);
/**
 * @brief 设置电机零点
 * @param motor 电机结构体指针
 */
HAL_StatusTypeDef QD4310_CAN_SetZeroPos(QD4310_CAN_t *motor);
/**
 * @brief 更新电机状态
 * @param motor 电机结构体指针
 * @param feedback 来自电机的反馈数据数组
 */
void QD4310_CAN_Update(QD4310_CAN_t *motor, const uint8_t feedback[8]);
/**
 * @brief 设置电机角度
 * @param motor 电机结构体指针
 * @param angle 设置的角度,[0,2pi]
 */
HAL_StatusTypeDef QD4310_CAN_SetAngle(QD4310_CAN_t *motor, float angle);
/**
 * @brief 设置电机步进角度
 * @param motor 电机结构体指针
 * @param step_angle 设置的角度,[-2pi,2pi]
 */
HAL_StatusTypeDef QD4310_CAN_SetStepAngle(QD4310_CAN_t *motor, float step_angle);
/**
 * @brief 设置电机转速
 * @param motor 电机结构体指针
 * @param speed 设置的转速,[-1000,1000]
 */
HAL_StatusTypeDef QD4310_CAN_SetSpeed(QD4310_CAN_t *motor, float speed);
/**
 * @brief 设置电机转速
 * @param motor 电机结构体指针
 * @param speed 设置的转速,[-1000,1000]
 */
HAL_StatusTypeDef QD4310_CAN_SetLowSpeed(QD4310_CAN_t *motor, float speed);
/**
 * @brief 设置电机电流
 * @param motor 电机结构体指针
 * @param current 设置的电流,[-10,10] A
 */
HAL_StatusTypeDef QD4310_CAN_SetCurrent(QD4310_CAN_t *motor, float current);

HAL_StatusTypeDef QD4310_CAN_SendCommand(QD4310_CAN_t *motor, QD4310_CAN_Command_t cmd, int16_t value);

// 数学常数定义
#define QD4310_CAN_PI (3.14159265358979323846f)
#define QD4310_CAN_TWO_PI (2.0f * QD4310_CAN_PI)

// 限制值宏定义
#define QD4310_CAN_MAX_SPEED (1000.0f)
#define QD4310_CAN_MIN_SPEED (-1000.0f)
#define QD4310_CAN_MAX_CURRENT (10.0f)
#define QD4310_CAN_MIN_CURRENT (-10.0f)
#define QD4310_CAN_MAX_STEPANGLE (QD4310_CAN_TWO_PI)
#define QD4310_CAN_MIN_STEPANGLE (-QD4310_CAN_TWO_PI)


#endif //QD4310_CAN_H
