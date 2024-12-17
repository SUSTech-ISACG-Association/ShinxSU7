#include "control.h"
#include "motor.h"

SU7State_t su7state = {0.4, 0.4, 0};

void goDirection(const direction_t dir, const float heading){
    float ang = Direction2float(dir) - heading;
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

void explore(){
    MOTOR_STOP();
}