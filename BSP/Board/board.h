#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    red_led,
    blue_led,
    green_led,
    purple_led,
    yellow_led,
    orange_led,
    led_off
} Board_Led_Color;

/* 初始化板载外设：启动 RGB 灯和蜂鸣器的 PWM，输出静默（灯灭、蜂鸣器停）。 */
void board_init(void);

/* 返回消抖后的 KEY 按下状态（20 ms 消抖，非阻塞）。 */
bool board_key_read(void);

/* 设置 RGB 灯颜色，参数非法返回 false。 */
bool set_board_led(Board_Led_Color color);

/* 非阻塞蜂鸣：freq_hz 为音调（20~20000 Hz），ms 为持续时长，到时自动停；
   ms 为 0 表示立即停止。 */
void board_beep(uint32_t freq_hz, uint32_t ms);
void board_buzzer_stop(void);

#endif
