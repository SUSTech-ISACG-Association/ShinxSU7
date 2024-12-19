#ifndef __MY_LIST_H
#define __MY_LIST_H

#include <stdlib.h>

typedef struct my_list
{
    void *data;
    struct my_list *next;
} my_list_t;

void lt_insert(my_list_t *p, void *data);
void lt_remove_next(my_list_t *p, void(*data_remover)(void *));
void lt_remove_all(my_list_t *p, void(*data_remover)(void *));

#endif