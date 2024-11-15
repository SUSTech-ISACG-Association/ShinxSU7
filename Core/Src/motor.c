#include "motor.h"

void SetMotorSpeed(unsigned char LR, unsigned char FB, signed char speed)
{
    if (LR == 0) {
        if (FB == 0) {
            SetMotorSpeedLF(speed);
        }
        else {
            SetMotorSpeedLB(speed);
        }
    }
    else {
        if (FB == 0) {
            SetMotorSpeedRF(speed);
        }
        else {
            SetMotorSpeedRB(speed);
        }
    }
}

void SetMotorSpeedLF(signed char speed)
{
    if (speed > 0) {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, speed);
        HAL_GPIO_WritePin(LEFT1_MOTOR_FORWARD_GPIO_Port, LEFT1_MOTOR_FORWARD_Pin, GPIO_PIN_RESET);
    }
    else {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, -speed);
        HAL_GPIO_WritePin(LEFT1_MOTOR_FORWARD_GPIO_Port, LEFT1_MOTOR_FORWARD_Pin, GPIO_PIN_SET);
    }
}
void SetMotorSpeedLB(signed char speed)
{
    if (speed > 0) {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, speed);
        HAL_GPIO_WritePin(LEFT2_MOTOR_FORWARD_GPIO_Port, LEFT2_MOTOR_FORWARD_Pin, GPIO_PIN_RESET);
    }
    else {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, -speed);
        HAL_GPIO_WritePin(LEFT2_MOTOR_FORWARD_GPIO_Port, LEFT2_MOTOR_FORWARD_Pin, GPIO_PIN_SET);
    }
}
void SetMotorSpeedRF(signed char speed)
{
    if (speed > 0) {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, speed);
        HAL_GPIO_WritePin(RIGHT1_MOTOR_FORWARD_GPIO_Port, RIGHT1_MOTOR_FORWARD_Pin, GPIO_PIN_RESET);
    }
    else {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, -speed);
        HAL_GPIO_WritePin(RIGHT1_MOTOR_FORWARD_GPIO_Port, RIGHT1_MOTOR_FORWARD_Pin, GPIO_PIN_SET);
    }
}
void SetMotorSpeedRB(signed char speed)
{

    if (speed > 0) {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, speed);
        HAL_GPIO_WritePin(RIGHT2_MOTOR_FORWARD_GPIO_Port, RIGHT2_MOTOR_FORWARD_Pin, GPIO_PIN_RESET);
    }
    else {
        __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, -speed);
        HAL_GPIO_WritePin(RIGHT2_MOTOR_FORWARD_GPIO_Port, RIGHT2_MOTOR_FORWARD_Pin, GPIO_PIN_SET);
    }
}