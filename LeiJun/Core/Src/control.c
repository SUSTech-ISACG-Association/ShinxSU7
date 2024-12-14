#include "control.h"
#ifndef V3
    #include "lcd_v4.h"
#else
    #include "lcd.h"
#endif
#include <stdlib.h>

uint16_t touch_state = 0;

uint16_t find_button(uint16_t x, uint16_t y, const list_t *ol){
    const list_t *pp = ol;
    while (pp != NULL) {
        if (((myobj_t *)(pp->data))->type == BUTTON) {
            const myobj_t *pw = pp->data;
            const img_t * im = get_obj_img(pw);
            if (pw->x <= x && x < pw->x + im->img_x && pw->y <= y &&
                y < pw->y + im->img_y) {
                return (uint16_t)(im->data[1]);
            }
        } else {
            break;
        }
        pp = pp->next;
    }
    return 0;
}

static uint16_t st_movx, st_movy;
static uint16_t las_movx, las_movy;
static uint8_t flag = 0;

void get_touch_state(const _m_tp_dev *p, const list_t *ol)
{
    if (p->sta & TP_PRES_DOWN) {
        if (p->x[0] < lcddev.width && p->y[0] < lcddev.height) {
            if (flag == 0) {
                uint16_t ww = find_button(p->x[0], p->y[0], ol);
                if (ww != 0) {
                    touch_state |= ww;
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
            int16_t dx = las_movx - st_movx;
            int16_t dy = las_movy - st_movy;

            if (-20 <= dx && dx <= 20 && -20 <= dy && dy <= 20) {
                touch_state |= find_button((las_movx+st_movx)>>1, (las_movy+st_movy)>>1, ol);
                flag = 0;
            } else if (dx >= dy && dx >= -dy){
                touch_state |= 0; // touch slide right
            } else if (dx <= dy && dx <= -dy){
                touch_state |= 0; // touch slide left
            } else if (dy >= dx && dy >= -dx){
                touch_state |= 0; // touch slide down
            } else {
                touch_state |= 0; // touch slide up
            }
        } else if (flag == 1) {
            touch_state |= find_button(st_movx, st_movy, ol);
        }
        flag = 0;
    }
}