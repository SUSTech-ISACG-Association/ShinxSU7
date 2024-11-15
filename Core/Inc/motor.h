#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "main.h"

extern TIM_HandleTypeDef htim4;

void SetMotorSpeed(unsigned char LR, unsigned char FB, signed char speed);

void SetMotorSpeedLF(signed char speed);
void SetMotorSpeedLB(signed char speed);
void SetMotorSpeedRF(signed char speed);
void SetMotorSpeedRB(signed char speed);

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
#define MOTOR_STOP                                                                                                     \
    do {                                                                                                               \
        SetMotorSpeedLF(0);                                                                                            \
        SetMotorSpeedLB(0);                                                                                            \
        SetMotorSpeedRF(0);                                                                                            \
        SetMotorSpeedRB(0);                                                                                            \
    } while (0)

#endif