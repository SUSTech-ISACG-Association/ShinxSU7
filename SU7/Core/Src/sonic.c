#include "sonic.h"

#include "delay.h"
#include "motor.h"
#include "sonic_motor.h"

extern float UltrasonicWave_Distance;

float UltrasonicWave_StartMeasure()
{
    // HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port, SONIC_WAVE_SEND_Pin, GPIO_PIN_SET);
    // HAL_Delay_us(20); // during here, RECV will be high and go into IRQ, until sonic received, RECV be low
    // HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port, SONIC_WAVE_SEND_Pin, GPIO_PIN_RESET);
    // return UltrasonicWave_Distance; // distance = 340 * T / 2
    
    HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port, SONIC_WAVE_SEND_Pin, GPIO_PIN_SET);
    HAL_Delay_us(12); // during here, RECV will be high and go into IRQ, until sonic received, RECV be low
    HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port, SONIC_WAVE_SEND_Pin, GPIO_PIN_RESET);
    return UltrasonicWave_Distance; // distance = 340 * T / 2
}

float SonicDetect(float angle)
{
    // uint32_t l1 = GetMotorSpeedLB(), l2 = GetMotorSpeedLF(), l3=GetMotorSpeedRB(), l4=GetMotorSpeedRF();
    SetSonicMotor(angle);
    HAL_Delay(200);
    float rt = UltrasonicWave_StartMeasure();
    // SetMotorSpeedLB(l1);
    // SetMotorSpeedLF(l2);
    // SetMotorSpeedRB(l3);
    // SetMotorSpeedRF(l4);
    return rt;
}