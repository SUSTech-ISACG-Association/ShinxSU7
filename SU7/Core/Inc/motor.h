#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "main.h"

extern TIM_HandleTypeDef htim4;

#define __SetMotorSpeed(__CHANNEL__, __PORT__, __PIN__, __speed__)                                                     \
    do {                                                                                                               \
        __HAL_TIM_SetCompare(&htim4, (__CHANNEL__), (72 * ((__speed__) < 0 ? -(__speed__) : 100 - (__speed__))));      \
        HAL_GPIO_WritePin((__PORT__), (__PIN__), ((__speed__) < 0 ? GPIO_PIN_SET : GPIO_PIN_RESET));                   \
    } while (0)
#define __GetMotorSpeed(__CHANNEL__) __HAL_TIM_GetCompare(&htim4, (__CHANNEL__))

#define SetMotorSpeedLF(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_1, LEFT1_MOTOR_FORWARD_GPIO_Port, LEFT1_MOTOR_FORWARD_Pin, ((speed) - 5.7))

#define SetMotorSpeedLB(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_2, LEFT2_MOTOR_FORWARD_GPIO_Port, LEFT2_MOTOR_FORWARD_Pin, ((speed) - 5.7))

#define SetMotorSpeedRF(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_3, RIGHT1_MOTOR_FORWARD_GPIO_Port, RIGHT1_MOTOR_FORWARD_Pin, speed)

#define SetMotorSpeedRB(speed)                                                                                         \
    __SetMotorSpeed(TIM_CHANNEL_4, RIGHT2_MOTOR_FORWARD_GPIO_Port, RIGHT2_MOTOR_FORWARD_Pin, speed)

#define GetMotorSpeedLF() __GetMotorSpeed(TIM_CHANNEL_1)

#define GetMotorSpeedLB() __GetMotorSpeed(TIM_CHANNEL_2)

#define GetMotorSpeedRF() __GetMotorSpeed(TIM_CHANNEL_3)

#define GetMotorSpeedRB() __GetMotorSpeed(TIM_CHANNEL_4)

#define MOTOR_SPINL(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(-(speed));                                                                                     \
        SetMotorSpeedLB(-(speed));                                                                                     \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_SPINR(speed)                                                                                             \
    MOTOR_SPINL(-(speed))

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
    MOTOR_FORWARD(-(speed))

#define MOTOR_LEFT(speed)                                                                                              \
    do {                                                                                                               \
        SetMotorSpeedLF(-(speed));                                                                                     \
        SetMotorSpeedLB(speed);                                                                                        \
        SetMotorSpeedRF(speed);                                                                                        \
        SetMotorSpeedRB(-(speed));                                                                                     \
    } while (0)

#define MOTOR_RIGHT(speed)                                                                                             \
    do {                                                                                                               \
        SetMotorSpeedLF(speed);                                                                                        \
        SetMotorSpeedLB(-(speed));                                                                                     \
        SetMotorSpeedRF(-(speed));                                                                                     \
        SetMotorSpeedRB(speed);                                                                                        \
    } while (0)

#define MOTOR_STOP()                                                                                                   \
    do {                                                                                                               \
        SetMotorSpeedLF(0);                                                                                            \
        SetMotorSpeedLB(0);                                                                                            \
        SetMotorSpeedRF(0);                                                                                            \
        SetMotorSpeedRB(0);                                                                                            \
    } while (0)

#define MOTOR_FORWARD_L(speed_forward, speed_L)                                                                        \
    do {                                                                                                               \
        SetMotorSpeedLF((speed_forward) - (speed_L));                                                                  \
        SetMotorSpeedLB((speed_forward) - (speed_L));                                                                  \
        SetMotorSpeedRF(speed_forward);                                                                                \
        SetMotorSpeedRB(speed_forward);                                                                                \
    } while (0)

#define MOTOR_FORWARD_R(speed_forward, speed_R)                                                                        \
    do {                                                                                                               \
        SetMotorSpeedLF(speed_forward);                                                                                \
        SetMotorSpeedLB(speed_forward);                                                                                \
        SetMotorSpeedRF((speed_forward) - (speed_R));                                                                  \
        SetMotorSpeedRB((speed_forward) - (speed_R));                                                                  \
    } while (0)

#define MOTOR_BACK_L(speed_back, speed_L)                                                                              \
    do {                                                                                                               \
        SetMotorSpeedLF(-(speed_back) + (speed_L));                                                                    \
        SetMotorSpeedLB(-(speed_back) + (speed_L));                                                                    \
        SetMotorSpeedRF(-(speed_back));                                                                                \
        SetMotorSpeedRB(-(speed_back));                                                                                \
    } while (0)

#define MOTOR_BACK_R(speed_back, speed_R)                                                                              \
    do {                                                                                                               \
        SetMotorSpeedLF(-(speed_back));                                                                                \
        SetMotorSpeedLB(-(speed_back));                                                                                \
        SetMotorSpeedRF(-(speed_back) + (speed_R));                                                                    \
        SetMotorSpeedRB(-(speed_back) + (speed_R));                                                                    \
    } while (0)

#endif