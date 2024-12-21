#include "bluetooth.h"
#include "gui.h"
#include <stdio.h>

#define MAXSPEED (100)

// #define WITH_BLUETOOTH

extern UART_HandleTypeDef *huart;
extern uint8_t LeiJun_mode;

// set by tim3 per 500 ms, sent within while(1) in main()
uint8_t need_greeting_flag = 0;

/**
 * @brief
 * for manual:
 * turnL	forward		turnR
 * left		 stop		right
 * rotL		 back		rotR
 * for waypoint and auto, map of 4 * 4
 */
uint16_t button_pressed = 0x0000;

uint8_t control_map[9][4] = {
    {0, 0, MAXSPEED, MAXSPEED},                   // Turn L
    {MAXSPEED, MAXSPEED, MAXSPEED, MAXSPEED},     // Forward
    {MAXSPEED, MAXSPEED, 0, 0},                   // Turn R
    {-MAXSPEED, MAXSPEED, MAXSPEED, -MAXSPEED},   // Left
    {0, 0, 0, 0},                                 // Stop
    {MAXSPEED, -MAXSPEED, -MAXSPEED, MAXSPEED},   // Right
    {-MAXSPEED, -MAXSPEED, MAXSPEED, MAXSPEED},   // RotL
    {-MAXSPEED, -MAXSPEED, -MAXSPEED, -MAXSPEED}, // Back
    {MAXSPEED, MAXSPEED, -MAXSPEED, -MAXSPEED}    // RotR
};

void handle_ack2(uint8_t ack2)
{
    uint8_t recv_buffer;
    float distance = 0;
    while (ack2 != 0x00) {
        if (ack2 >= 0xf0) {
            char warning[50];
            sprintf(warning, "error ack2=%x", ack2);
            display_info(warning);
            break;
        }
        HAL_UART_Transmit(huart, &ack2, 1, 0xffff);
        HAL_UART_Receive(huart, &recv_buffer, 1, 0xffff); // ack1
        switch (ack2) {
        case 0x80:
            HAL_UART_Receive(huart, &distance, 4, 0xffff);
            break;
        case 0x81:
            // get position
            break;
        default:
            char warning[50];
            sprintf(warning, "error ack2=%x", ack2);
            display_info(warning);
            break;
        }
        HAL_UART_Receive(huart, &ack2, 1, 0xffff); // ack2
    }
}

void send_set_mode(uint8_t mode)
{
#ifdef WITH_BLUETOOTH
    uint8_t receive_buffer;
    // after consume the distance, send mode switching msg
    // 1: manual control
    // 2: waypoint control
    // 3: auto detect
    // 4: auto race
    HAL_UART_Transmit(huart, &mode, 1, 0xffff);
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer);
#endif
}

float get_distance()
{

    uint8_t opcode = 0x00;
    uint8_t receive_buffer;
    float distance = 0;
#ifdef WITH_BLUETOOTH
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff);        // send Greeting
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer);
    opcode = 0x80;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff);        // send GET distance
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK1
                                                         // TODO: handle error ACK1
    HAL_UART_Receive(huart, &distance, 4, 0xffff);       // recv distance data
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer);
#endif
    return distance;
}

void send_manual_inst()
{
#ifdef WITH_BLUETOOTH
    uint8_t opcode = 0x10;
    uint8_t receive_buffer;
    for (size_t i = 0; i < 9; i++) {
        if (button_pressed & (1 << i)) {
            HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
            HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
            HAL_UART_Transmit(huart, control_map[i], 4, 0xffff);
            break;
        }
    }
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
    handle_ack2(receive_buffer);
#endif
}

void send_waypoint()
{
#ifdef WITH_BLUETOOTH
    uint8_t opcode = 0x00;
    uint8_t receive_buffer;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff);        // send greeting
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer);
    opcode = 0x20;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff);        // send waypoint opcode
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK1
    // TODO: handle error code of ACK1
    waypoint_list[waypoint_cnt] = 0xff;
    HAL_UART_Transmit(huart, waypoint_list, waypoint_cnt + 1, 0xffff);
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
    handle_ack2(receive_buffer);
#endif
}

void send_toggle_run()
{
#ifdef WITH_BLUETOOTH
    uint8_t opcode = is_running ? 0x05 : 0x06;
    uint8_t receive_buffer;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
    handle_ack2(receive_buffer);
#endif
}