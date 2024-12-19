#ifndef _OBJECTS_H
#define _OBJECTS_H

#include "touch.h"
#include <stdint.h>

typedef struct button_area_t {
    uint16_t sx, sy, ex, ey;
} button_area;

extern button_area button_manual[9];
extern button_area button_map[16];
extern button_area button_auto[2];
extern uint8_t LeiJun_mode;

uint16_t which_button_pressed_manual(_m_tp_dev *p);
uint16_t which_button_pressed_waypoint(_m_tp_dev *p);
uint16_t which_button_pressed_auto(_m_tp_dev *p);

#endif
