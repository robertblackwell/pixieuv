#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "simple_buffer.h"

#include "blk_message.h"

#define PARSER_STATE_INITIAL  11
#define PARSER_STATE_LENGTH   22
#define PARSER_STATE_BODY     33


blk_message_t* bm_create()
{
    char* tmp = malloc(sizeof(blk_message_t));
    blk_message_t* p = (blk_message_t*) tmp;
    p->msg_buf = sb_create();
    p->length = 0;
    p->header = sb_create();
    p->body = sb_create();
    p->body_offset = 0;
    
    return p;
}

void bm_free(blk_message_t* m)
{
    sb_free(m->msg_buf);
    sb_free(m->header);
    sb_free(m->body);
}
