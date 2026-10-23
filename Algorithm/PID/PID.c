#include "PID.h"

/*
 * PD 环
 */
int32_t pid_Calculate_PD(int32_t param0, int32_t kp, int32_t kd, int32_t param1)
{
    return (kp * param0) - (kd * param1);
}

/*
 * PI 环
 */
float pid_CalculatePI(float param0 , float kp , float ki , float *integral, float integral_limit)
{
    float new_integral;
    float p_term;
    float i_term;

    new_integral = *integral + (ki  * param0);

    if (new_integral > integral_limit)
    {
        new_integral = integral_limit;
    }
    if (new_integral < -integral_limit)
    {
        new_integral = -integral_limit;
    }

    *integral = new_integral;

    p_term = (kp  * param0) ;
    i_term = new_integral ;

    return p_term + i_term;
}
