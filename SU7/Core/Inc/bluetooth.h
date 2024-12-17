#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"

extern uint8_t proto_code;
extern UART_HandleTypeDef *huart;

void set_bluetooth_huart(UART_HandleTypeDef* h);
void start_bluetooth_IT();
void stop_bluetooth_IT();

void bluetooth_RxCallback();

#endif