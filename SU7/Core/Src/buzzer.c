#include "buzzer.h"

#define TIM_FREQ 72000000

static TIM_HandleTypeDef *buzzer_tim_handler;

void set_buzzer_tim_handler(TIM_HandleTypeDef *htim) { buzzer_tim_handler = htim; }

void start_buzzer()
{
    HAL_TIM_PWM_Start(buzzer_tim_handler, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(buzzer_tim_handler, TIM_CHANNEL_1, 950);
}

void stop_buzzer()
{
    // HAL_TIM_PWM_Stop(buzzer_tim_handler, TIM_CHANNEL_1);
    // __HAL_TIM_SET_COMPARE(buzzer_tim_handler, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(buzzer_tim_handler, TIM_CHANNEL_1, 1000 + 1);
}

void set_buzzer_frequency(float frequency)
{
    if (frequency < 1e-3) {
        __HAL_TIM_SET_PRESCALER(buzzer_tim_handler, 0);
    }
    else {
        __HAL_TIM_SET_PRESCALER(buzzer_tim_handler, (uint32_t)((TIM_FREQ / (1000 * frequency)) - 0.5));
    }
}

void bark(float frequency, int duration)
{
    set_buzzer_frequency(frequency);
    start_buzzer();
    HAL_Delay(duration);
    stop_buzzer();
}