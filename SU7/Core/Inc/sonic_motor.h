#ifndef _SONIC_MOTOR_H_
#define _SONIC_MOTOR_H_

#include "main.h"

extern TIM_HandleTypeDef htim3;

void SetSonicMotor(float angle);

#endif