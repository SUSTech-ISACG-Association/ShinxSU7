#include "objects.h"

/**
 * [0-8]	button_manual: turn * 2, rotate * 2, move * 4, stop * 1
 * [9-24]	button_map: 16 * grids for setting waypoints/obstacles/start/end
 * [25-26]	button_auto: auto-avoid + auto-race
 */
button_area button_manual[9], button_map[16], button_auto[2];

extern uint8_t LeiJun_mode;

#define WITHIN_BUTTON(x, y, button_instance)                                                                           \
    ((((button_instance.sx) <= (x)) && ((x) <= (button_instance.ex))) ||                                               \
     (((button_instance.sy) <= (y)) && ((y) <= (button_instance.ey))))

uint16_t which_button_pressed_manual(_m_tp_dev *p)
{
    uint16_t result = 0x0000;
    if (p->sta & TP_PRES_DOWN) {
        for (uint8_t i = 0; i < 9; i++) {
            if (WITHIN_BUTTON(p->x[0], p->y[0], button_manual[i])) {
                result |= 1 << i;
            }
        }
    }
    return result;
}

uint16_t which_button_pressed_waypoint(_m_tp_dev *p)
{
    uint16_t result = 0x0000;
    if (p->sta & TP_PRES_DOWN) {
        for (uint8_t i = 0; i < 16; i++) {
            if (WITHIN_BUTTON(p->x[0], p->y[0], button_map[i])) {
                result |= 1 << i;
            }
        }
    }
    return result;
}
uint16_t which_button_pressed_auto(_m_tp_dev *p)
{
    uint16_t result = 0x0000;
    if (p->sta & TP_PRES_DOWN) {
        for (uint8_t i = 0; i < 2; i++) {
            if (WITHIN_BUTTON(p->x[0], p->y[0], button_auto[i])) {
                result |= 1 << i;
            }
        }
    }
    return result;
}