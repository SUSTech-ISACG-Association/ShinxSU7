#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

extern UART_HandleTypeDef *huart;
extern uint16_t button_pressed;

void send_greeting();
void send_mode();
uint8_t send_manual_inst();

#endif