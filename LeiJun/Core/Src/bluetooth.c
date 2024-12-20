#include "bluetooth.h"
#include "gui.h"
#include <stdio.h>

extern UART_HandleTypeDef *huart;
extern uint8_t LeiJun_mode;

// set by tim3 per 500 ms, sent within while(1) in main()
uint8_t need_greeting_flag = 0;



void handle_ack2(uint8_t ack2)
{
    uint8_t recv_buffer;
    float distance = 0;
    while (ack2 != 0x00) {
        if (ack2 >= 0xf0) {
            // show_warning_message
            break;
        }
        HAL_UART_Transmit(huart, &ack2, 1, 0xffff);
        HAL_UART_Receive(huart, &recv_buffer, 1, 0xffff); // ack1

        switch (ack2) {
        case 0x80:
            HAL_UART_Receive(huart, &distance, 1, 0xffff);
            draw_manual(distance);
            break;
        case 0x81:
            // get position
            break;
        default:
            // show_warning_message
            break;
        }
        HAL_UART_Receive(huart, &ack2, 1, 0xffff); // ack2
    }
}

float get_distance()
{
    uint8_t opcode = 0x00;
    uint8_t receive_buffer;
    float distance = 0;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff); // send Greeting
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer); // TODO: handle error ACK2
    opcode = 0x80;
    HAL_UART_Transmit(huart, &opcode, 1, 0xffff); // send GET distance
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK1
                                                         // TODO: handle error ACK1
    HAL_UART_Receive(huart, &distance, 4, 0xffff); // recv distance data
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff); // recv ACK2
    handle_ack2(receive_buffer);
    return distance;
}
// manual
// turnL	forward		turnR
// left		 stop		right
// rotL		 back		rotR
uint16_t button_pressed = 0x0000;
uint8_t send_manual_inst()
{
    uint8_t opcode = 0x00;
    uint8_t receive_buffer;
    if (button_pressed & (1 << 0)) // turn L
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {0, 0, 100, 100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & (1 << 1)) // forward
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {100, 100, 100, 100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // turn R
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {100, 100, 0, 0};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // left
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {-100, 100, 100, -100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // stop
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {0, 0, 0, 0};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // right
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {100, -100, -100, 100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // rotL
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {-100, -100, 100, 100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // back
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {0, 0, 0, 0};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    else if (button_pressed & 0b0100) // rotR
    {
        opcode = 0x10;
        HAL_UART_Transmit(huart, &opcode, 1, 0xffff);
        HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
        // LF, LB, RF, RB
        uint8_t control[4] = {100, 100, -100, -100};
        HAL_UART_Transmit(huart, control, 4, 0xffff);
    }
    HAL_UART_Receive(huart, &receive_buffer, 1, 0xffff);
    return 0x00;
}
