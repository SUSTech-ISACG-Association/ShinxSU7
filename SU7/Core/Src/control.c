#include "control.h"
#include "motor.h"
#include "scene.h"

SU7State_t su7state = {0, 0, 0};

void goDirection(const direction_t dir, const direction_t heading){
    float ang = Direction2float(dir) - Direction2float(heading);
    if (ang < 0) {
        MOTOR_SPINR(-ang);
        HAL_Delay(1530/360*(-ang));
    } else {
        MOTOR_SPINL(ang);
        HAL_Delay(1530/360*ang);
    }
    MOTOR_FORWARD(100);
    HAL_Delay(1.7);
    MOTOR_STOP();
}

void autoavoid_update(){

}

typedef enum {
    FOLLOWING,
    TURNL_WAITING,
    TURNL,
    TURNR_WAITING,
    TURNR
} RaceState_t;


static RaceState_t race_state;

void autorace_update(){
    uint8_t now_stateL = HAL_GPIO_ReadPin(SEARCH_L_GPIO_Port, SEARCH_L_Pin) == GPIO_PIN_SET;
    uint8_t now_stateM = HAL_GPIO_ReadPin(SEARCH_M_GPIO_Port, SEARCH_M_Pin) == GPIO_PIN_SET;
    uint8_t now_stateR = HAL_GPIO_ReadPin(SEARCH_R_GPIO_Port, SEARCH_R_Pin) == GPIO_PIN_SET;
    uint8_t now_state = (now_stateL << 2) | (now_stateM << 1) | now_stateR;
    switch (now_state)
    {
    case 0b000:
        if (race_state == TURNL_WAITING || race_state == TURNL) {
            race_state = TURNL;
        } else if (race_state == TURNR_WAITING || race_state == TURNR) {
            race_state = TURNR;
        } else {
            race_state = FOLLOWING;
        }
        break;
    case 0b001:
        race_state = TURNR;
        break;
    case 0b010:
        // TODO: 回正
        race_state = FOLLOWING;
        break;
    case 0b011:
        race_state = TURNR_WAITING;
        break;
    case 0b100:
        race_state = TURNL;
        break;
    case 0b101:
        break;
    case 0b110:
        race_state = TURNL_WAITING;
        break;
    case 0b111:
        if (race_state == TURNL) {
            race_state = TURNR;
        } else if (race_state == TURNR) {
            race_state = TURNL;
        }
        break;
    
    default:
        break;
    }
    if (race_state == TURNL) {
        MOTOR_FORWARD_L(100, 20);
    } else if (race_state == TURNR) {
        MOTOR_FORWARD_R(100, 20);
    } else {
        MOTOR_FORWARD(100);
    }
}

void control_init(){
    su7state = (SU7State_t){0, 0, 0};
    Scene_destroy(&ShinxScene1);
    Scene_init(&ShinxScene1);
}