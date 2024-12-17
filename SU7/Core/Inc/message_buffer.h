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

my_message_t* new_my_message(uint8_t code, uint8_t *data, uint16_t length);

void free_my_message(void*);

extern my_list_t message_buffer_header;

void append_my_message(uint8_t code, uint8_t *data, uint16_t length);

my_message_t* get_my_message();

void release_one_message();

my_message_t *find_message(uint8_t code);

void remove_last_find_message();

#endif