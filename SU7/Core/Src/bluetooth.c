#include "bluetooth.h"
#include "control.h"
#include "main.h"
#include "message_buffer.h"
#include "motor.h"
#include "scene.h"
#include "sonic.h"

void set_bluetooth_huart(UART_HandleTypeDef *h) { huart = h; }

void start_bluetooth_IT() { HAL_UART_Receive_IT(huart, &proto_code, 1); }

void stop_bluetooth_IT() { HAL_UART_AbortReceive_IT(huart); }

uint8_t proto_code, flag = 0;
uint32_t proto_buffer;
UART_HandleTypeDef *huart = NULL;

void bluetooth_sendACK1(uint8_t ack_code) { HAL_UART_Transmit(huart, &ack_code, 1, 0xffff); }
void bluetooth_sendACK2(uint8_t ack_code)
{
    if (ack_code != 0x00) {
        HAL_UART_Transmit(huart, &ack_code, 1, 0xffff);
    }
    else {
        my_message_t *p = get_my_message();
        if (p != NULL) {
            HAL_UART_Transmit(huart, &p->code, 1, 0xffff);
        }
        else {
            HAL_UART_Transmit(huart, &ack_code, 1, 0xffff);
        }
    }
}

// uint32_t ctEnEl;
// uint32_t ctExEl;

void bluetooth_RxCallback()
{
    my_message_t *p;

#ifndef NDEBUG
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#endif

    switch (proto_code) {
    case 0x00:
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    case 0x01:
        bluetooth_sendACK2(set_control_mode());
        start_bluetooth_IT();
        break;
    case 0x02:
        bluetooth_sendACK2(set_waypoint_mode());
        start_bluetooth_IT();
        break;
    case 0x03:
        bluetooth_sendACK2(set_autopilot_mode());
        start_bluetooth_IT();
        break;
    case 0x04:
        bluetooth_sendACK2(set_auto_race_mode());
        start_bluetooth_IT();
        break;
    case 0x05:
        start_mode();
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    case 0x06:
        end_mode();
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    case 0x10:
        if (flag == 0) {
            bluetooth_sendACK1(0x00);
            HAL_UART_Receive_IT(huart, (uint8_t *)&proto_buffer, 4);
            flag = 1;
        }
        else {
            if (su7mode == CONTROL_MODE) {
                int8_t spd = *((int8_t *)(&proto_buffer));
                SetMotorSpeedLF(spd);
                spd = *(((int8_t *)(&proto_buffer)) + 1);
                SetMotorSpeedLB(spd);
                spd = *(((int8_t *)(&proto_buffer)) + 2);
                SetMotorSpeedRF(spd);
                spd = *(((int8_t *)(&proto_buffer)) + 3);
                SetMotorSpeedRB(spd);
            }
            flag = 0;
            bluetooth_sendACK2(0x00);
            start_bluetooth_IT();
        }
        break;
    case 0x20:
        bluetooth_sendACK1(0x00);
        uint8_t cc;
        HAL_UART_Receive(huart, &cc, 1, 0xffff);
        while (cc != 0xff){
            Scene_add_waypoint(&ShinxScene1, (Waypoint){cc & 0x3, cc >> 2});
            HAL_UART_Receive(huart, &cc, 1, 0xffff);
        }
        bluetooth_sendACK2(0x00);
        // su7state = (SU7State_t){ShinxScene1.waypoints.arr[0], Y_NEGATIVE};
        start_bluetooth_IT();
        flag = 0;
        break;
    case 0x30:
        bluetooth_sendACK1(0x00);
        HAL_UART_Receive(huart, (uint8_t *)&proto_buffer, 2, 0xffff);
        uint8_t st = *((uint8_t *)(&proto_buffer));
        uint8_t en = *((uint8_t *)(&proto_buffer)+1);
        set_autopilot_position((Waypoint){st & 0x3, st >> 2}, (Waypoint){en & 0x3, en >> 2});
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    case 0x80:
        bluetooth_sendACK1(0x00);
        HAL_Delay(1);
        p = find_message(0x80); // Get the message
        if (p != NULL) {
            HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
            remove_last_find_message();
        }
        else {
            float dis = FastSonicDetect(1, 1000); // sonic priority: 0, UART priority: 1
            HAL_UART_Transmit(huart, (uint8_t *)&dis, sizeof(float), 0xffff);
        }
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    case 0x81:
        bluetooth_sendACK1(0x00);
        HAL_Delay(1);
        p = find_message(0x81);
        if (p != NULL) {
            HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
            remove_last_find_message();
        }
        else {
            proto_buffer = 0xffffffff;
            HAL_UART_Transmit(huart, (uint8_t *)&proto_buffer, 1, 0xffff);
        }
        HAL_Delay(1);
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    
    case 0x82:
        p = find_message(0x82);
        if (p == NULL)
        {
            // Send NACK
            bluetooth_sendACK1(0xFF);
            break;
        }
        bluetooth_sendACK1(0x00);
        HAL_Delay(1);
        HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
        // HAL_UART_Transmit(huart, &ctEnEl, sizeof(uint32_t), 0xffff);
        remove_last_find_message();
        HAL_Delay(1);
        p = find_message(0x82);
        HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
        // HAL_UART_Transmit(huart, &ctExEl, sizeof(uint32_t), 0xffff);
        remove_last_find_message();

        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
        // TODO: Add handler for other codes
    case 0x90:
        bluetooth_sendACK1(0x00);
        HAL_Delay(1);
        p = find_message(0x90);
        if (p == NULL)
        {
            uint8_t nn = 0xff;
            nn = (su7state.heading << 4) | (su7state.pos.y << 2) | su7state.pos.x;
            HAL_UART_Transmit(huart, &nn, 1, 0xffff);
            HAL_Delay(1);
        } else {
            HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
            HAL_Delay(1);
            // HAL_UART_Transmit(huart, &ctEnEl, sizeof(uint32_t), 0xffff);
            remove_last_find_message();
        }
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;

    default:
        start_bluetooth_IT();
        break;
    }
}