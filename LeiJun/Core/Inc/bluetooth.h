#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

extern UART_HandleTypeDef *huart;
extern uint16_t button_pressed;

float get_distance();
uint8_t send_manual_inst();
void send_waypoint();

#endif