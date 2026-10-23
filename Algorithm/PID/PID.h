#ifndef PID_H
#define PID_H

#include <stdint.h>

/* 纯算法接口 */

/*pid*/
int32_t pid_Calculate_PD(int32_t param0, int32_t kp, int32_t kd, int32_t param1);
float pid_CalculatePI(float param0 , float kp , float ki , float *integral, float integral_limit);

#endif
