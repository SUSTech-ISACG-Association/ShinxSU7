#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "main.h"

extern TIM_HandleTypeDef htim4;

void SetMotorSpeed(unsigned char LR, unsigned char FB, signed char speed);

#define __SetMotorSpeed(__CHANNEL__, __PORT__, __PIN__, __speed__)                                                     \
    do {                                                                                                               \
        __HAL_TIM_SetCompare(&htim4, (__CHANNEL__), (72 * ((__speed__) < 0 ? -(__speed__) : 100 - (__speed__))));           \
        HAL_GPIO_WritePin((__PORT__), (__PIN__), ((__speed__) < 0 ? GPIO_PIN_SET : GPIO_PIN_RESET));                   \
    } while (0)

#define SetMotorSpeedLF(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_1, LEFT1_MOTOR_FORWARD_GPIO_Port, LEFT1_MOTOR_FORWARD_Pin, speed)

#define SetMotorSpeedLB(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_2, LEFT2_MOTOR_FORWARD_GPIO_Port, LEFT2_MOTOR_FORWARD_Pin, speed)

#define SetMotorSpeedRF(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_3, RIGHT1_MOTOR_FORWARD_GPIO_Port, RIGHT1_MOTOR_FORWARD_Pin, speed)

#define SetMotorSpeedRB(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_4, RIGHT2_MOTOR_FORWARD_GPIO_Port, RIGHT2_MOTOR_FORWARD_Pin, speed)

#define MOTOR_SPINL(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(-speed);                                                                                       \
        SetMotorSpeedLB(-speed);                                                                                       \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_SPINR(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(speed);                                                                                        \
        SetMotorSpeedLB(speed);                                                                                        \
        SetMotorSpeedRF(-speed);                                                                                       \
        SetMotorSpeedRB(-speed);                                                                                       \
    } while (0)

#define MOTOR_TURNL(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(0);                                                                                            \
        SetMotorSpeedLB(0);                                                                                            \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_TURNR(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(speed);                                                                                        \
        SetMotorSpeedLB(speed);                                                                                        \
        SetMotorSpeedRF(0);                                                                                            \
        SetMotorSpeedRB(0);                                                                                            \
    } while (0)

#define MOTOR_FORWARD(speed)                                                                                           \
    do {                                                                                                               \
        SetMotorSpeedLF(speed);                                                                                        \
        SetMotorSpeedLB(speed);                                                                                        \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_BACK(speed)                                                                                              \
    do {                                                                                                               \
        SetMotorSpeedLF(-speed);                                                                                       \
        SetMotorSpeedLB(-speed);                                                                                       \
        SetMotorSpeedRF(-speed);                                                                                       \
        SetMotorSpeedRB(-speed);                                                                                       \
    } while (0)

#define MOTOR_LEFT(speed)                                                                                              \
    do {                                                                                                               \
        SetMotorSpeedLF(-speed);                                                                                       \
        SetMotorSpeedLB(speed);                                                                                        \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(-speed);                                                                                       \
    } while (0)

#define MOTOR_RIGHT(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(speed);                                                                                        \
        SetMotorSpeedLB(-speed);                                                                                       \
        SetMotorSpeedRF(-speed);                                                                                       \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_STOP()                                                                                                   \
    do {                                                                                                               \
        SetMotorSpeedLF(0);                                                                                            \
        SetMotorSpeedLB(0);                                                                                            \
        SetMotorSpeedRF(0);                                                                                            \
        SetMotorSpeedRB(0);                                                                                            \
    } while (0)

#endif