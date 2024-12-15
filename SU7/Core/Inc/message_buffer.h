#ifndef __MESSAGE_BFFER_H
#define __MESSAGE_BFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "my_list.h"

typedef struct my_message
{
    uint8_t code;
    uint8_t *data;
    uint16_t length;
} my_message_t;

inline my_message_t* new_my_message(uint8_t code, uint8_t *data, uint16_t length) {
    my_message_t *p = (my_message_t*)malloc(sizeof(my_message_t));
    p->code = code;
    p->length = length;
    p->data = (uint8_t*)malloc(sizeof(uint8_t)*length);
    memcpy(p->data, data, length);
    return p;
}

void free_my_message(void*);

extern my_list_t message_buffer_header;

inline void append_my_message(uint8_t code, uint8_t *data, uint16_t length) {
    lt_insert(&message_buffer_header, new_my_message(code, data, length));
}

inline my_message_t* get_my_message(){
    return message_buffer_header.next == NULL ? NULL : (my_message_t*)(message_buffer_header.next->data);
}

inline void release_one_message(){
    lt_remove_next(&message_buffer_header, free_my_message);
}

my_message_t *find_message(uint8_t code);

void remove_last_find_message();

#endif