#ifndef _CONTROL_H
#define _CONTROL_H

#include <stdint.h>
#include "touch.h"
#include "objects.h"

#define MOVE_FORWARD 0x01
#define MOVE_BACK 0x02
#define MOVE_LEFT 0x04
#define MOVE_RIGHT 0x08

#define SPIN_LEFT 0x10
#define SPIN_RIGHT 0x20
#define STOP 0x40

void get_touch_state(const _m_tp_dev *p, const list_t *ol);

#endif