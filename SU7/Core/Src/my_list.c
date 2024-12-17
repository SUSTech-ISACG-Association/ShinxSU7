#include "my_list.h"

void lt_insert(my_list_t *p, void *data) {
    if (p == NULL) return;
    my_list_t *o = (my_list_t*)malloc(sizeof(my_list_t));
    o->next = p->next;
    o->data = data;
    p->next = o;
}

void lt_remove_next(my_list_t *p, void(*data_remover)(void *)) {
    if (p == NULL) return;
    my_list_t *o = p->next;
    if (o == NULL) return;
    p->next = o->next;
    data_remover(o->data);
    free(o);
}

void lt_remove_all(my_list_t *p, void(*data_remover)(void *)) {
    while(p->next != NULL) {
        lt_remove_next(p, data_remover);
    }
}