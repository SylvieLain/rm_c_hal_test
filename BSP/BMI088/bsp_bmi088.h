#ifndef __BSP_BMI088_H
#define __BSP_BMI088_H

#ifdef __cplusplus
extern "C" {
#endif

#include "struct_typedef.h"

/* 读芯片 ID：ACC 应为 0x1E，GYRO 应为 0x0F */
uint8_t bsp_bmi088_read_accel_id(void);
uint8_t bsp_bmi088_read_gyro_id(void);

/* 读取一次数据：加速度 g、角速度 deg/s、温度 ℃ */
void bsp_bmi088_read(fp32 gyro[3], fp32 accel[3], fp32 *temperature);

/* 初始化（含自检），返回 0 表示成功，其他为错误位 */
uint8_t bsp_bmi088_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_BMI088_H */
