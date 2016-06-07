#ifndef BLK_PARSER_H
#define BLK_PARSER_H

#include <stdio.h>
#include "simple_buffer.h"
#include "blk_message.h"

struct blk_parser_s;
typedef struct blk_parser_s blk_parser_t;

typedef void (blk_parser_cb)(blk_parser_t* parser, blk_message_t* msg, int status);

struct blk_parser_s{
    blk_message_t*      msg_ptr;
    int                 state;
    long                position_in_message;
    blk_parser_cb       *cb;
    void*               data;
};


void bmp_init(blk_parser_t * p, blk_parser_cb cb);

void bmp_append(blk_parser_t* p, char* bytes, int count);

#endif /* parser_h */
