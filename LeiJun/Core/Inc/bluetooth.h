#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

extern UART_HandleTypeDef *huart;
extern uint16_t button_pressed;
extern uint8_t updated;

/**
 * @brief send instruction setting to the desired mode
 * 
 * @param mode 
 */
void send_set_mode(uint8_t mode);

/**
 * @brief consume ack2, then positively get distance of the obstacle ahead
 * 
 * @return float 4-byte distance
 */
float get_distance();

/**
 * @brief send instructions according to manual control
 * 
 */
void send_manual_inst();

/**
 * @brief send waypoint list to SU7
 * 
 */
void send_waypoint();

/**
 * @brief to run or to stop, this is a question
 * 
 */
void send_toggle_run();

#endif