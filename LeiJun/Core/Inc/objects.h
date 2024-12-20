#ifndef _OBJECTS_H
#define _OBJECTS_H

#include "touch.h"
#include <stdint.h>

#define WITHIN_BUTTON(x, y, button_instance)                                                                           \
    ((((button_instance.sx) <= (x)) && ((x) <= (button_instance.ex))) ||                                               \
     (((button_instance.sy) <= (y)) && ((y) <= (button_instance.ey))))

typedef struct button_area_t {
    uint16_t sx, sy, ex, ey;
} button_area;

extern button_area button_manual[9];
extern button_area button_map[16];
extern button_area button_go, button_clear;
extern button_area button_auto[2];
extern uint8_t LeiJun_mode;

/**
 * @brief get the button pressed in manual mode as a uint16_t bitmap
 *
 * @param p pointer to the touch panel device
 * @return uint16_t bitmap of the button pressed[0-8]
 */
uint16_t which_button_pressed_manual(_m_tp_dev *p);

/**
 * @brief get the button pressed in waypoint mode as a uint16_t bitmap
 *
 * @param p pointer to the touch panel device
 * @return uint16_t bitmap of the button pressed[0-15]
 */
uint16_t which_button_pressed_waypoint(_m_tp_dev *p);

/**
 * @brief get the button pressed in auto mode as a uint16_t bitmap
 *
 * @param p pointer to the touch panel device
 * @return uint16_t bitmap of the button pressed[0-1]
 */
uint16_t which_button_pressed_auto(_m_tp_dev *p);

#endif
