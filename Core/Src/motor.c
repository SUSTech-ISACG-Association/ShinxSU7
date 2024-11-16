#include "motor.h"

void SetMotorSpeed(unsigned char LR, unsigned char FB, signed char speed)
{
    if (LR == 0) {
        if (FB == 0) {
            SetMotorSpeedLF(speed);
        }
        else {
            SetMotorSpeedLB(speed);
        }
    }
    else {
        if (FB == 0) {
            SetMotorSpeedRF(speed);
        }
        else {
            SetMotorSpeedRB(speed);
        }
    }
}
