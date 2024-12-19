#ifndef _OBJECTS_H
#define _OBJECTS_H

#include <stdint.h>

typedef struct list
{
    void* data;
    struct list *next;
} list_t;

extern list_t obj_list_head;

void insert_lt(list_t *p, void *o);
void free_lt(list_t *p);
void remove_lt(list_t *p);

typedef struct img{
    const uint16_t *data;
    uint16_t img_x, img_y, scale;
} img_t;

typedef enum object_type{
    OBSTACLE,
    BUTTON,
} object_type_t;

typedef struct myobj{
    int16_t x, y;
    img_t *data;
    object_type_t type;
} myobj_t;

const img_t *get_obj_img(const myobj_t* o);
myobj_t* new_myobj(uint16_t x, uint16_t y, object_type_t ot);

#endif
