#include "draw.h"
#include "objects.h"
#include "lcd.h"

#include <stddef.h>
#include <string.h>

#define FONT_SIZE 12

int16_t mymin(int16_t a, int16_t b){
    return a<b ? a : b;
}
int16_t mymax(int16_t a, int16_t b){
    return a>b ? a : b;
}

void draw_elements(const myobj_t *p){
    const img_t * im = get_obj_img(p);
    if(p->x > lcddev.width || p->y > lcddev.height) return;
    if(p->x+im->img_x < 0 || p->y+im->img_y < 0) return;

        int16_t las_y = 32000;
        int16_t dx = mymin((int16_t)lcddev.height, (int16_t)(p->y + im->img_y*im->scale)), dy = mymin((int16_t)lcddev.width, (int16_t)(p->x + im->img_x*im->scale));
        uint16_t i=0, j=0;
        uint16_t di=0, dj=0, ddi=p->y, ddj=p->x;
        int16_t ux = -p->x;

        if (p->y < 0.01) {
            int16_t uy = -p->y;
            di = uy%im->scale;
            i = uy/im->scale;
            ddi = 0;
        }
        for(int16_t ii=ddi;ii<dx;++ii){
            
            if (ux >= 0) {
                dj = ux%im->scale;
                j = ux/im->scale;
                ddj = 0;
            } else {
                dj = 0;
                j = 0;
                ddj = -ux;
            }
            uint16_t idx = i*im->img_x;
            las_y = 32000;
            for(uint16_t jj=ddj;jj<dy;++jj){
                uint16_t idy = idx + j;
                if(1){
                    POINT_COLOR = im->data[i*im->img_x+j];
                    if (las_y + 1 != ii){
                        LCD_DrawPoint(jj,ii);
                        las_y = ii;
                    } else {
                        LCD_WR_DATA(POINT_COLOR);
                        ++ las_y;
                    }
                }
                if ((++dj) == im->scale) {
                    dj=0;
                    ++j;
                }
            }
            if ((++di) == im->scale) {
                di=0;
                ++i;
            }
        }
}

void draw_elements_notrans(const myobj_t *p) {
    if (p->type == OBSTACLE){
        draw_elements(p);
        return;
    }
    const img_t * im = get_obj_img(p);
    if(p->x >= lcddev.width || p->y >= lcddev.height) return;
    if(p->x+im->img_x <= 0 || p->y+im->img_y <= 0) return;
    
    int16_t height, width;
    int16_t npx = p->x, npy = p->y;
    uint16_t i, j;
    width = mymin(im->img_x * im->scale + npx, lcddev.width);
    height = mymin(im->img_y * im->scale + npy, lcddev.height); // 高度
    uint16_t ddx = npx < 0 ? 0 : npx;
    uint16_t ddy = npy < 0 ? 0 : npy;
    if (width <= 0 || height <= 0) return;
    if (ddx > width || ddy > height) {
        LCD_ShowString(20, 160, 300, 12, 12, "invalid drawing ");
        LCD_ShowNum(0, 160, p->type, 2, 12);
        return;
    }
    LCD_Set_Window(ddx, ddy, width-ddx, height-ddy);
    LCD_SetCursor(ddx, ddy);  // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    for (i = ddy; i < height; i++) {
        for (j = ddx; j < width; j++)
            LCD_WR_DATA(im->data[(j-npx)/im->scale + im->img_x * (i-npy)/im->scale]); // 写入数据
    }
    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);
}


void draw_UI(const myobj_t *p){
    const img_t* im = get_obj_img(p);
    LCD_Fill_Window(mymax(p->x, 0), mymax(p->y, 0), mymin(p->x + im->img_x - 1, lcddev.width-1), mymin(p->y + im->img_y - 1, lcddev.height-1), im->data[0]);
    const char *ss = im->data;
    size_t lss = strlen(ss);
    uint16_t dx = (im->img_x - lss * (FONT_SIZE>>1))>>1;
    uint16_t dy = (im->img_y - FONT_SIZE)>>1;
    LCD_ShowString_trans(p->x + dx, p->y + dy, lss * ((FONT_SIZE>>1)+1), FONT_SIZE, FONT_SIZE, ss);
}

void draw_all_FDP(const list_t *ol){
    const list_t *p = ol;
        
    while(p != NULL){
        if (((myobj_t*)(p->data))->type != BUTTON){
            draw_elements_notrans(p->data);
        }
        p=p->next;
    }
    p = ol;
    POINT_COLOR = RED;
    while(p != NULL){
        if (((myobj_t*)(p->data))->type == BUTTON){
            draw_UI(p->data);
        }
        p=p->next;
    }
}

void draw_all(const list_t *ol){
    draw_all_FDP(ol);
}
