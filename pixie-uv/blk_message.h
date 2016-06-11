#ifndef BLK_MESSAGE_H
#define BLK_MESSAGE_H

#include <stdio.h>
#include "simple_buffer.h"

struct blk_message_s;
typedef struct blk_message_s blk_message_t;

struct blk_message_s{
    simple_buffer_t*    msg_buf;
    simple_buffer_t*    header;
    int                 length; // value of length bytes in message
//    simple_buffer_t*    body;
    int                 body_offset; // start of body in message buffer
};

blk_message_t* bm_create();

void bm_free(blk_message_t* m);

#endif /* parser_h */
