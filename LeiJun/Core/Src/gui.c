#include "gui.h"
#include <stdio.h>
#include <string.h>

extern uint8_t start, end;
extern uint8_t LeiJun_mode;
extern uint8_t waypoint_state;
extern uint16_t obstacles = 0x0000;
extern uint8_t waypoint_list[16];

void draw_button_text(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t text_size, const char *text,
                      button_area *button)
{
    button->ex = ex;
    button->ey = ey;
    button->sx = sx;
    button->sy = sy;
    LCD_Fill_Window(sx, sy, ex, ey, BUTTON_EDGE_COLOR);
    LCD_Fill_Window(sx + 1, sy + 1, ex - 1, ey - 1, BUTTON_BACK_COLOR);
    uint16_t text_sx = sx + (((ex - sx) - (strlen(text) * text_size >> 1)) >> 1);
    uint16_t text_sy = sy + (((ey - sy) - text_size) >> 1);
    POINT_COLOR = BUTTON_TEXT_COLOR;
    LCD_ShowString_trans(text_sx, text_sy, text_size * strlen(text) >> 1, text_size, text_size, text);
}

void draw_button_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t color, button_area *button)
{
    button->ex = ex;
    button->ey = ey;
    button->sx = sx;
    button->sy = sy;
    LCD_Fill_Window(sx, sy, ex, ey, INVERT_RGB565(color));
    LCD_Fill_Window(sx + 1, sy + 1, ex - 1, ey - 1, color);
}

void draw_waypoint_objects()
{
    for (int i = 0; i < 16; i++) {
        if (start == i) {
            draw_button_text(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, 16, "S",
                             (button_map + i));
        }
        else if (end == i) {
            draw_button_text(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, 16, "E",
                             (button_map + i));
        }
        else if (obstacles & (1 << i)) {
            draw_button_color(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, RED,
                              (button_map + i));
        }
        else {
            draw_button_color(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, BUTTON_BACK_COLOR,
                              (button_map + i));
        }
    }
	// TODO: draw arrows of waypoint list.
}

void draw_manual(float distance)
{
    LCD_ShowString(64, 10, 150, 16, 16, "Manual Control");
    draw_button_text(30, 60, 90, 120, 16, "TurnL", (button_manual + 0));
    draw_button_text(90, 60, 150, 120, 16, "Forward", (button_manual + 1));
    draw_button_text(150, 60, 210, 120, 16, "TurnR", (button_manual + 2));
    draw_button_text(30, 120, 90, 180, 16, "Left", (button_manual + 3));
    draw_button_text(90, 120, 150, 180, 16, "Stop", (button_manual + 4));
    draw_button_text(150, 120, 210, 180, 16, "Right", (button_manual + 5));
    draw_button_text(30, 180, 90, 240, 16, "RotL", (button_manual + 6));
    draw_button_text(90, 180, 150, 240, 16, "Back", (button_manual + 7));
    draw_button_text(150, 180, 210, 240, 16, "RotR", (button_manual + 8));
    if (distance >= 0) { // negative distance means no obstacle detected, do not update
        char distance_buffer[50];
        sprintf(distance_buffer, "Distance: %.1f cm", distance);
        LCD_Fill_Window(10, 250, 100, 267, WHITE);
        POINT_COLOR = BLACK;
        LCD_ShowString(10, 250, 200, 16, 16, distance_buffer);
    }
}

void draw_waypoint()
{
    LCD_ShowString(56, 10, 150, 16, 16, "Waypoint Control");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            draw_button_color(20 + j * 50, 60 + i * 50, 70 + j * 60, 110 + i * 50, BLACK, (button_map + i * 4 + j));
        }
    }
    char mode_text[20];
    if (waypoint_state == 0) {
        sprintf(mode_text, "Set Start");
    }
    else if (waypoint_state == 1) {
        sprintf(mode_text, "Set End");
    }
    else if (waypoint_state == 2) {
        sprintf(mode_text, "Set Obstacles");
    }
    else if (waypoint_state == 3) {
        sprintf(mode_text, "Set Waypoints");
    }
    POINT_COLOR = BLACK;
    LCD_ShowString(10, 270, 200, 16, 16, mode_text);
    draw_button_text(120, 270, 160, 310, 16, "Clear", &button_clear);
    draw_button_text(180, 270, 220, 310, 16, "Go", &button_go);
    draw_waypoint_objects();
}