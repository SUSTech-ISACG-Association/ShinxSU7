#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"

extern uint8_t proto_code;
extern UART_HandleTypeDef *huart;

inline void set_bluetooth_huart(UART_HandleTypeDef* h) {
    huart = h;
}

inline void start_bluetooth_IT(){
    HAL_UART_Receive_IT(huart, &proto_code, 1);
}

inline void stop_bluetooth_IT() {
    HAL_UART_AbortReceive_IT(huart);
}

void bluetooth_RxCallback();

#endif