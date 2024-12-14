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

typedef struct img {
    const uint16_t *data;
    const uint16_t *mask;
    uint16_t img_x, img_y, scale;
} img_t;


typedef struct myobj{
    uint16_t x, y;
    img_t *data;
} myobj_t;

#endif
