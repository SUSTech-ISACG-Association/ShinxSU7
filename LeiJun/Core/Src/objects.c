#include "objects.h"
#include "control.h"
#include "main.h"
#include "lcd.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdlib.h>

extern float speed_x, speed_y;
extern float total_x, end_position;

void insert_lt(list_t *p, void *o){
    if(p==NULL){
        return;
    }
    list_t *pp = p->next;
    p->next = (list_t*)malloc(sizeof(list_t));
    p->next->data = o;
    p->next->next = pp;
}

void free_obj(myobj_t *p){
    if(p == NULL) return;
    free(p->data);
    free(p);
}
void free_lt(list_t *p){
    if (p == NULL) return;
    free_obj(p->data);
    free_lt(p->next);
    free(p);
}
void remove_lt(list_t *p){
    if(p == NULL) return;
    if(p->next == NULL) return;
    list_t *pp = p->next->next;
    p->next->next = NULL;
    free_lt(p->next);
    p->next = pp;
}

const img_t *get_obj_img(const myobj_t* o){
    return &(o->data);
}

myobj_t* new_myobj(uint16_t x, uint16_t y, object_type_t ot){
    myobj_t *p = (myobj_t*)malloc(sizeof(myobj_t));
    p->type = ot;
    p->x = x;
    p->y = y;
    if (ot == OBSTACLE){
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->data = (uint16_t* ) obstacle_image;
        p->data->img_x = p->data->img_y = 72;
        p->data->scale = 1;
    } else if (ot == BUTTON) {
        p->data = (img_t*)malloc(sizeof(img_t));
        p->data->scale = 1;
    }
    return p;
}
