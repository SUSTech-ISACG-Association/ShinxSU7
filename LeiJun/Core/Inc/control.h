#ifndef _CONTROL_H
#define _CONTROL_H

#include <stdint.h>
#include "touch.h"
#include "objects.h"

#define MOVE_UP 0x01
#define MOVE_DOWN 0x02
#define MOVE_FORWARD 0x04
#define MOVE_BACK 0x08

#define DASH_M 0x10
#define CROUCH_M 0x20
#define JUMP_M 0x40


#define LOGIC_SKIP 2
#define SQ_LOGIC_SKIP 1.2
extern const float MAX_X_SPEED;
extern const float X_SPEED_DEC;
extern const float X_SPEED_DELTA;

extern const float Y_SPEED_JUMP;
extern const float Y_SPEED_UP;
extern const float Y_SPEED_DEC;

#define CROUCH_FRAC 0.75


#endif
