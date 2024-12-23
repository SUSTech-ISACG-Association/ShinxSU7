#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f1xx_hal.h"

void set_buzzer_tim_handler(TIM_HandleTypeDef *htim);
void start_buzzer();
void stop_buzzer();
void set_buzzer_frequency(float frequency);
void bark(float frequency, int duration);

#endif // BUZZER_H