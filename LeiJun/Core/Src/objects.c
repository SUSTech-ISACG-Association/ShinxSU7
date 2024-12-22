#include "objects.h"

/**
 * [0-8]	button_manual: turn * 2, rotate * 2, move * 4, stop * 1
 * [9-24]	button_map: 16 * grids for setting waypoints/obstacles/start/end
 */
button_area button_manual[9], button_map[16], button_clear, button_go, button_mode;

uint8_t start = 0xff, end = 0xff;

/**
 * @brief bitmap of obstacles, 1 means obstacle, 0 means no obstacle
 *
 */
uint16_t obstacles = 0x0000;

uint8_t waypoint_state;
uint8_t waypoint_list[100];
uint8_t waypoint_cnt = 0;

uint8_t LeiJun_mode;

uint8_t whereami = 0, face_direction = 0, is_race = 0, is_running = 0, is_setting_start = 1;

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

uint16_t which_button_pressed_map(_m_tp_dev *p)
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

void reset_states()
{
    start = 0xff;
    end = 0xff;
    obstacles = 0x0000;
    waypoint_state = 0;
    waypoint_cnt = 0;
    whereami = 0;
    face_direction = 0;
    is_running = is_race = 0;
    is_setting_start = 1;
}
