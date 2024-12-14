#include "control.h"
#include "lcd_v4.h"
#include <stdlib.h>

uint16_t movements = 0;
uint16_t movements_buffer;

static uint16_t st_movx, st_movy, las_movx, las_movy;
static uint8_t flag;

extern float speed_x, speed_y;
extern float total_x, end_position;

extern uint8_t sig_end;

uint16_t find_button(uint16_t x, uint16_t y, const list_t *ol){
    const list_t *pp = ol;
    while (pp != NULL) {
        const myobj_t *pw = pp->data;
        const img_t * im = get_obj_img(pw);
        if (pw->x-0.01 <= x && x < pw->x+0.01 + im->img_x && pw->y-0.01 <= y &&
            y < pw->y+0.01 + im->img_y) {
            return (uint16_t)(im->data[1]);
        }
        pp = pp->next;
    }
    return 0;
}

void update_movement(const _m_tp_dev *p, const list_t *ol)
{
    if (p->sta & TP_PRES_DOWN) {
        if (p->x[0] < lcddev.width && p->y[0] < lcddev.height) {
            if (flag == 0) {
                uint16_t ww = find_button(p->x[0], p->y[0], ol);
                if (ww != 0) {
                    movements |= ww;
                    return;
                }
                st_movx = p->x[0];
                st_movy = p->y[0];
                flag = 1;
            }
            else if (flag >= 1) {
                las_movx = p->x[0];
                las_movy = p->y[0];
                flag = 2;
            } else {
                flag = 0;
            }
        }
    }
    else {
        if (flag == 2) {
            int16_t dx = (las_movx - st_movx)*2;
            int16_t dy = las_movy - st_movy;

            if (-40 <= dx && dx <= 40 && -20 <= dy && dy <= 20) {
                movements |= find_button((las_movx+st_movx)>>1, (las_movy+st_movy)>>1, ol);
                flag = 0;
            } else if (dx >= dy && dx >= -dy){
                movements |= MOVE_FORWARD;
            } else if (dx <= dy && dx <= -dy){
                movements |= MOVE_BACK;
            } else if (dy >= dx && dy >= -dx){
                movements |= MOVE_DOWN;
            } else {
                movements |= MOVE_UP;
            }
        } else if (flag == 1) {
            movements |= find_button(st_movx, st_movy, ol);
        }
        flag = 0;
    }
}
