#include "board.h"
#include "main.h"
#include "tim.h"

/* 硬件映射 (RoboMaster C 板)：
   KEY    PA0        输入上拉，低电平按下
   LED_B  PH10 TIM5_CH1  LED_G PH11 TIM5_CH2  LED_R PH12 TIM5_CH3
   RGB 灯为共阳极，低电平点亮
   BUZZER PD14 TIM4_CH3  无源蜂鸣器，PWM 驱动 */

#define LED_TIM_HANDLE     htim5
#define LED_TIM_CH_R       TIM_CHANNEL_3
#define LED_TIM_CH_G       TIM_CHANNEL_2
#define LED_TIM_CH_B       TIM_CHANNEL_1

#define BUZZER_TIM_HANDLE  htim4
#define BUZZER_TIM_CH      TIM_CHANNEL_3

/* TIM4 预分频 167，计数时钟 84 MHz / 168 = 500 kHz */
#define BUZZER_CNT_CLK_HZ  500000U
#define BEEP_FREQ_MIN_HZ   20U
#define BEEP_FREQ_MAX_HZ   20000U

bool board_key_read(void)
{
    static bool last;
    static bool pressed;
    static uint32_t changed_ms;
    uint32_t now = HAL_GetTick();
    bool raw = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET;

    if (raw != last)
    {
        last = raw;
        changed_ms = now;
    }
    if (now - changed_ms >= 20U)
    {
        pressed = raw;
    }
    return pressed;
}

/* 设置单通道亮度 0-255，共阳极低电平点亮，占空比取反 */
static void led_set_brightness(uint32_t channel, uint8_t level)
{
    uint32_t duty = (uint32_t)level * 65535U / 255U;
    __HAL_TIM_SET_COMPARE(&LED_TIM_HANDLE, channel, 65535U - duty);
}

bool set_board_led(Board_Led_Color color)
{
    uint8_t r, g, b;

    switch (color)
    {
        case red_led:    r = 255; g = 0;   b = 0;   break;
        case blue_led:   r = 0;   g = 0;   b = 255; break;
        case green_led:  r = 0;   g = 255; b = 0;   break;
        case purple_led: r = 255; g = 0;   b = 255; break;
        case yellow_led: r = 255; g = 255; b = 0;   break;
        case orange_led: r = 255; g = 128; b = 0;   break;
        case led_off:    r = 0;   g = 0;   b = 0;   break;
        default: return false;
    }

    led_set_brightness(LED_TIM_CH_R, r);
    led_set_brightness(LED_TIM_CH_G, g);
    led_set_brightness(LED_TIM_CH_B, b);
    return true;
}

static bool beep_active;
static uint32_t beep_end_ms;

void board_buzzer_stop(void)
{
    /* PWM1：占空比 0 输出恒低，蜂鸣器静音 */
    __HAL_TIM_SET_COMPARE(&BUZZER_TIM_HANDLE, BUZZER_TIM_CH, 0U);
    beep_active = false;
}

void board_beep(uint32_t freq_hz, uint32_t ms)
{
    uint32_t arr;

    if (ms == 0U)
    {
        board_buzzer_stop();
        return;
    }
    if (freq_hz < BEEP_FREQ_MIN_HZ) freq_hz = BEEP_FREQ_MIN_HZ;
    if (freq_hz > BEEP_FREQ_MAX_HZ) freq_hz = BEEP_FREQ_MAX_HZ;

    arr = BUZZER_CNT_CLK_HZ / freq_hz - 1U;
    __HAL_TIM_SET_AUTORELOAD(&BUZZER_TIM_HANDLE, arr);
    __HAL_TIM_SET_COMPARE(&BUZZER_TIM_HANDLE, BUZZER_TIM_CH, (arr + 1U) / 2U);

    beep_end_ms = HAL_GetTick() + ms;
    beep_active = true;
}

/* 蜂鸣定时停止挂在 1 ms SysTick 上，无需业务层轮询 */
void HAL_SYSTICK_Callback(void)
{
    if (beep_active && (int32_t)(HAL_GetTick() - beep_end_ms) >= 0)
    {
        board_buzzer_stop();
    }
}

void board_init(void)
{
    HAL_TIM_PWM_Start(&LED_TIM_HANDLE, LED_TIM_CH_R);
    HAL_TIM_PWM_Start(&LED_TIM_HANDLE, LED_TIM_CH_G);
    HAL_TIM_PWM_Start(&LED_TIM_HANDLE, LED_TIM_CH_B);
    HAL_TIM_PWM_Start(&BUZZER_TIM_HANDLE, BUZZER_TIM_CH);

    board_buzzer_stop();
    set_board_led(led_off);
}
