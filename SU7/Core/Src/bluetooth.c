#include "bluetooth.h"
#include "message_buffer.h"
#include "motor.h"
#include "sonic.h"

uint8_t proto_code, flag=0;
uint32_t proto_buffer;
UART_HandleTypeDef *huart = NULL;

void bluetooth_sendACK1(uint8_t ack_code){
    HAL_UART_Transmit(huart, &ack_code, 1, 0xffff);
}
void bluetooth_sendACK2(uint8_t ack_code){
    if (ack_code != 0x00) {
        HAL_UART_Transmit(huart, &ack_code, 1, 0xffff);
    } else {
        my_message_t *p = get_my_message();
        if (p != NULL) {
            HAL_UART_Transmit(huart, &p->code, 1, 0xffff);
        } else {
            HAL_UART_Transmit(huart, &ack_code, 1, 0xffff);
        }
    }
}

void bluetooth_RxCallback(){
    switch (proto_code)
    {
    case 0x00:
        bluetooth_sendACK2(0x00); // TODO: any error code
        start_bluetooth_IT();
        break;
    case 0x01: //TODO, set to control mode
        break;
    case 0x10:
        if (flag == 0) {
            bluetooth_sendACK1(0x00);
            HAL_UART_Receive_IT(huart, &proto_buffer, 4);
            flag = 1;
        } else {
            uint8_t spd = (proto_buffer&0xff000000)>>24;
            SetMotorSpeedLF(spd);
            spd = (proto_buffer&0x00ff0000)>>16;
            SetMotorSpeedLB(spd);
            spd = (proto_buffer&0x0000ff00)>>8;
            SetMotorSpeedRF(spd);
            spd = (proto_buffer&0x000000ff)>>0;
            SetMotorSpeedRB(spd);
            flag = 0;
            bluetooth_sendACK2(0x00);
            start_bluetooth_IT();
        }
        break;
    case 0x11:
        if (flag == 0) {
            bluetooth_sendACK1(0x00);
            HAL_UART_Receive_IT(huart, &proto_buffer, 4);
            flag = 1;
        } else {
            uint8_t spd = *((uint8_t*)(&proto_buffer));
            SetMotorSpeedLF(spd);
            flag = 0;
            bluetooth_sendACK2(0x00);
            start_bluetooth_IT();
        }
        break;
    case 0x80:
        bluetooth_sendACK1(0x00);
        my_message_t *p = find_message(0x80);
        if(p != NULL){
            HAL_UART_Transmit(huart, p->data, p->length, 0xffff);
            remove_last_find_message();
        } else {
            float dis = SonicDetect(90); // sonic priority: 0, UART priority: 1
            HAL_UART_Transmit(huart, &dis, p->length, 0xffff);
        }
        bluetooth_sendACK2(0x00);
        start_bluetooth_IT();
        break;
    // TODO: finish this
    
    default:
        break;
    }
}