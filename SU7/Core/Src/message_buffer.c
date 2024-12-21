#include "message_buffer.h"

my_list_t message_buffer_header={NULL, NULL};

void free_my_message(void *p){
    free(((my_message_t*)p)->data);
    free((my_message_t*)p);
}

static my_list_t *las_find = NULL;

my_message_t *find_message(uint8_t code){
    my_list_t *p = message_buffer_header.next;
    las_find = &message_buffer_header;
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

my_message_t* new_my_message(uint8_t code, uint8_t *data, uint16_t length) {
    my_message_t *p = (my_message_t*)malloc(sizeof(my_message_t));
    p->code = code;
    p->length = length;
    p->data = (uint8_t*)malloc(sizeof(uint8_t)*length);
    memcpy(p->data, data, length);
    return p;
}

void append_my_message(uint8_t code, uint8_t *data, uint16_t length) {
    lt_insert(&message_buffer_header, new_my_message(code, data, length));
}

my_message_t* get_my_message(){
    return message_buffer_header.next == NULL ? NULL : (my_message_t*)(message_buffer_header.next->data);
}

void release_one_message(){
    lt_remove_next(&message_buffer_header, free_my_message);
}
