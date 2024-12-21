#include "gui.h"
#include <stdio.h>
#include <string.h>

extern uint8_t LeiJun_mode;

extern uint8_t waypoint_state;
extern uint8_t start, end;
extern uint16_t obstacles;
extern uint8_t waypoint_cnt;
extern uint8_t waypoint_list[100];

extern uint8_t whereami, face_direction, is_race, is_running;

void draw_button_text(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint8_t text_size, const char *text,
                      button_area *button)
{
    button->ex = ex;
    button->ey = ey;
    button->sx = sx;
    button->sy = sy;
    LCD_Fill_Window(sx, sy, ex, ey, BUTTON_BACK_COLOR);
    uint16_t text_sx = sx + (((ex - sx) - (strlen(text) * text_size >> 1)) >> 1);
    uint16_t text_sy = sy + (((ey - sy) - text_size) >> 1);
    POINT_COLOR = BUTTON_TEXT_COLOR;
    LCD_ShowString_trans(text_sx, text_sy, text_size * strlen(text) >> 1, text_size, text_size, (uint8_t *)text);
}

void draw_button_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t color, button_area *button)
{
    button->ex = ex;
    button->ey = ey;
    button->sx = sx;
    button->sy = sy;
    LCD_Fill_Window(sx, sy, ex, ey, color);
}

void draw_manual()
{
    POINT_COLOR = WHITE;
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
    for (int i = 0; i < 4; i++) {
        POINT_COLOR = BUTTON_EDGE_COLOR;
        LCD_DrawLine(30, 60 + i * 60, 210, 60 + 60 * i);
        LCD_DrawLine(30 + 60 * i, 60, 30 + 60 * i, 240);
    }
}

void display_info(char *buf)
{
    LCD_Fill_Window(10, 290, 110, 302, BACK_COLOR);
    POINT_COLOR = WHITE;
    LCD_ShowString(10, 290, 110, 12, 12, (uint8_t *)buf);
}

void draw_waypoint_path()
{
    for (int i = 0; i + 1 < waypoint_cnt; i++) {
        int st = waypoint_list[i];
        int ed = waypoint_list[i + 1];
        int start_x = (button_map[st].sx + button_map[st].ex) >> 1;
        int start_y = (button_map[st].sy + button_map[st].ey) >> 1;
        int end_x = (button_map[ed].sx + button_map[ed].ex) >> 1;
        int end_y = (button_map[ed].sy + button_map[ed].ey) >> 1;
        if (start_x == end_x) {
            start_x -= 1;
            end_x += 1;
        }
        if (start_y == end_y) {
            start_y -= 1;
            end_y += 1;
        }
        LCD_Fill_Window(start_x < end_x ? start_x : end_x, start_y < end_y ? start_y : end_y,
                        start_x > end_x ? start_x : end_x, start_y > end_y ? start_y : end_y, GREEN);
    }
    // draw arrows for the last waypoint
    for (int it = 2; it <= waypoint_cnt; it++) {
        int st = waypoint_list[it - 2];
        int ed = waypoint_list[it - 1];
        uint16_t ed_x = (button_map[ed].sx + button_map[ed].ex) >> 1;
        uint16_t ed_y = (button_map[ed].sy + button_map[ed].ey) >> 1;
        POINT_COLOR = GREEN;
        int dir[4] = {0, 0, 0, 0};
        if (st + 1 == ed) {
            // right
            dir[0] = -5;
            dir[1] = -5;
            dir[2] = -5;
            dir[3] = 5;
        }
        else if (st == ed + 1) {
            // left
            dir[0] = 5;
            dir[1] = 5;
            dir[2] = 5;
            dir[3] = -5;
        }
        else if (st + 4 == ed) {
            // down
            dir[0] = -5;
            dir[1] = -5;
            dir[2] = 5;
            dir[3] = -5;
        }
        else if (st == ed + 4) {
            // up
            dir[0] = -5;
            dir[1] = 5;
            dir[2] = 5;
            dir[3] = 5;
        }
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                LCD_DrawLine(ed_x + i, ed_y + j, ed_x + i + dir[0], ed_y + j + dir[1]);
                LCD_DrawLine(ed_x + i, ed_y + j, ed_x + i + dir[2], ed_y + j + dir[3]);
            }
        }
    }
}

void draw_waypoint_objects()
{
    for (int i = 0; i < 16; i++) {
        if (start == i) {
            POINT_COLOR = WHITE;
            draw_button_text(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, 24, "S",
                             (button_map + i));
        }
        else if (end == i) {
            POINT_COLOR = WHITE;
            draw_button_text(button_map[i].sx, button_map[i].sy, button_map[i].ex, button_map[i].ey, 24, "E",
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
    for (int i = 0; i < 5; i++) {
        POINT_COLOR = BUTTON_EDGE_COLOR;
        LCD_DrawLine(20, 40 + 50 * i, 220, 40 + 50 * i);
        LCD_DrawLine(20 + 50 * i, 40, 20 + 50 * i, 240);
    }
    draw_waypoint_path();
}

void draw_waypoint()
{
    POINT_COLOR = WHITE;
    LCD_ShowString(56, 10, 150, 16, 16, "Waypoint Control");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            draw_button_color(20 + j * 50, 40 + i * 50, 70 + j * 50, 90 + i * 50, BLACK, (button_map + i * 4 + j));
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
    uint8_t mode_text_x = 120 + ((100 - strlen(mode_text) * 6) >> 1);
    LCD_Fill_Window(120, 244, 220, 256, BACK_COLOR);
    POINT_COLOR = WHITE;
    LCD_ShowString(mode_text_x, 244, 84, 12, 12, mode_text);
    draw_button_text(120, 260, 170, 310, 16, "Clear", &button_clear);
    draw_button_text(170, 260, 220, 310, 16, "Go", &button_go);
    LCD_DrawLine(120, 260, 220, 260);
    LCD_DrawLine(120, 310, 220, 310);
    LCD_DrawLine(120, 260, 120, 310);
    LCD_DrawLine(170, 260, 170, 310);
    LCD_DrawLine(220, 260, 220, 310);
    draw_waypoint_objects();
}

void draw_auto()
{
    POINT_COLOR = WHITE;
    LCD_ShowString(84, 10, 72, 16, 16, "Autopilot");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i * 4 + j == whereami) {
                draw_button_text(20 + j * 50, 40 + i * 50, 70 + j * 50, 90 + i * 50, 24, "Me",
                                 (button_map + i * 4 + j));
            }
            else {
                draw_button_color(20 + j * 50, 40 + i * 50, 70 + j * 50, 90 + i * 50, BLACK, (button_map + i * 4 + j));
            }
        }
    }
    for (int i = 0; i < 5; i++) {
        POINT_COLOR = BUTTON_EDGE_COLOR;
        LCD_DrawLine(20, 40 + 50 * i, 220, 40 + 50 * i);
        LCD_DrawLine(20 + 50 * i, 40, 20 + 50 * i, 240);
    }
    draw_button_text(120, 260, 170, 310, 16, "Mode", &button_mode);
    draw_button_text(170, 260, 220, 310, 16, "Go", &button_go);
    LCD_DrawLine(120, 260, 220, 260);
    LCD_DrawLine(120, 310, 220, 310);
    LCD_DrawLine(120, 260, 120, 310);
    LCD_DrawLine(170, 260, 170, 310);
    LCD_DrawLine(220, 260, 220, 310);
    char mode_text[20];
	LCD_Fill_Window(120, 244, 220, 256, BACK_COLOR);
    sprintf(mode_text, is_race ? "Race" : "Detect");
    uint8_t mode_text_x = 120 + ((100 - strlen(mode_text) * 6) >> 1);
    POINT_COLOR = is_running ? GREEN : WHITE;
    LCD_ShowString(mode_text_x, 244, 84, 12, 12, mode_text);
}

void drawUI()
{
    switch (LeiJun_mode) {
    case 0:
        draw_manual();
        break;
    case 1:
        draw_waypoint();
        break;
    case 2:
        draw_auto();
        break;
    default:
        break;
    }
}