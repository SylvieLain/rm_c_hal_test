#include "bsp_pwm.h"
#include "tim.h"

static const struct
{
    TIM_HandleTypeDef *tim;
    uint32_t channel;
} pwm[] = {
    {&htim1, TIM_CHANNEL_1},  /* C1 PE9 */
    {&htim1, TIM_CHANNEL_2},  /* C2 PE11 */
    {&htim1, TIM_CHANNEL_3},  /* C3 PE13 */
    {&htim1, TIM_CHANNEL_4},  /* C4 PE14 */
    {&htim8, TIM_CHANNEL_1},  /* C5 PC6 */
    {&htim8, TIM_CHANNEL_2},  /* C6 PI6 */
    {&htim8, TIM_CHANNEL_3},  /* C7 PI7 */
};

HAL_StatusTypeDef bsp_pwm_init(void)
{
    HAL_StatusTypeDef status;

    if (htim1.State == HAL_TIM_STATE_RESET || htim8.State == HAL_TIM_STATE_RESET)
        return HAL_ERROR;

    for (uint8_t i = 0; i < 7; i++)
        __HAL_TIM_SET_COMPARE(pwm[i].tim, pwm[i].channel, 0);

    for (uint8_t i = 1; i <= 7; i++)
    {
        status = bsp_pwm_start(i);
        if (status != HAL_OK) return status;
    }
    return HAL_OK;
}

HAL_StatusTypeDef bsp_pwm_start(uint8_t channel)
{
    if (channel < 1 || channel > 7) return HAL_ERROR;
    return HAL_TIM_PWM_Start(pwm[channel - 1].tim, pwm[channel - 1].channel);
}

HAL_StatusTypeDef bsp_pwm_stop(uint8_t channel)
{
    if (channel < 1 || channel > 7) return HAL_ERROR;
    __HAL_TIM_SET_COMPARE(pwm[channel - 1].tim, pwm[channel - 1].channel, 0);
    return HAL_TIM_PWM_Stop(pwm[channel - 1].tim, pwm[channel - 1].channel);
}

HAL_StatusTypeDef bsp_pwm_set_pulse(uint8_t channel, uint32_t pulse_us)
{
    if (channel < 1 || channel > 7) return HAL_ERROR;
    TIM_HandleTypeDef *tim = pwm[channel - 1].tim;
    if (pulse_us > __HAL_TIM_GET_AUTORELOAD(tim) + 1U) return HAL_ERROR;
    __HAL_TIM_SET_COMPARE(tim, pwm[channel - 1].channel, pulse_us);
    return HAL_OK;
}

HAL_StatusTypeDef bsp_pwm_set_duty(uint8_t channel, uint16_t duty)
{
    if (channel < 1 || channel > 7 || duty > 1000) return HAL_ERROR;
    TIM_HandleTypeDef *tim = pwm[channel - 1].tim;
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(tim) + 1U;
    __HAL_TIM_SET_COMPARE(tim, pwm[channel - 1].channel, period * duty / 1000U);
    return HAL_OK;
}
