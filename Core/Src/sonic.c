#include "sonic.h"

#include "delay.h"
#include "sonic_motor.h"

extern float UltrasonicWave_Distance;

float UltrasonicWave_StartMeasure()
{
  HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port,SONIC_WAVE_SEND_Pin, GPIO_PIN_SET);
  HAL_Delay_us(20);
  HAL_GPIO_WritePin(SONIC_WAVE_SEND_GPIO_Port,SONIC_WAVE_SEND_Pin, GPIO_PIN_RESET);
  return UltrasonicWave_Distance;
}

float SonicDetect(float angle){
    SetSonicMotor(angle);
    HAL_Delay(200);
    return UltrasonicWave_StartMeasure();
}