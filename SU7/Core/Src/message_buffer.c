#include "message_buffer.h"

my_list_t message_buffer_header={NULL, NULL};

void free_my_message(void *p){
    free(((my_message_t*)p)->data);
    free((my_message_t*)p);
}

static my_list_t *las_find = NULL;

my_message_t *find_message(uint8_t code){
    my_list_t *p = message_buffer_header.next;
    while(p != NULL){
        if (((my_message_t*)(p->data))->code == code) {
            return p->data;
        }
        las_find = p;
        p = p->next;
    }
    las_find = NULL;
    return NULL;
}

void remove_last_find_message(){
    lt_remove_next(las_find, free_my_message);
}