#include "sonic_motor.h"

void SetSonicMotor(float angle)
{
    angle = (uint16_t)(50.0 * (angle*18.0/18.5+8.2) / 9.0 + 249.0);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, angle);
}