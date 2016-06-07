#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "simple_buffer.h"

#include "blk_message.h"
#include "blk_parser.h"

#define PARSER_STATE_INITIAL  11
#define PARSER_STATE_LENGTH   22
#define PARSER_STATE_BODY     33


void bmp_init(blk_parser_t* p, blk_parser_cb cb)
{
    p->msg_ptr = bm_create();
    p->position_in_message = 0;
    p->state = PARSER_STATE_INITIAL;
    p->cb = cb;
}

void bpm_reset(blk_parser_t* p){
}

/**
 * A message is of the form
 *
 * START/nLEN/nBODY ...........
 *
 *  The LEN field is the length of the body. The body is composed on 8-bit bytes
 */
void bmp_append(blk_parser_t* p, char* bytes, int count)
{
    
    bool error = false;
    bool done = false;
    char* start_str = "START";
    long start_length = strlen(start_str);
    
    int     i = 0;

    for(; i < count ;)
    {
        switch(p->state)
        {
            case PARSER_STATE_INITIAL  :
                for(; p->state == PARSER_STATE_INITIAL ;){
                    if( (bytes[i] == start_str[p->position_in_message]) && (p->position_in_message <= start_length) ){
                        p->state = PARSER_STATE_INITIAL;
                        error = false;
                        // append byte t message
                        sb_append(p->msg_ptr->header, &bytes[i], 1);
                        sb_append(p->msg_ptr->msg_buf, &bytes[i], 1);
                        i++; p->position_in_message++;
                        
                    }else if( (p->position_in_message == start_length) && (bytes[i] == '\n') ){
                        p->state = PARSER_STATE_LENGTH;
                        error = false;
                        // append byte to message and save the position of the \n
                        sb_append(p->msg_ptr->msg_buf, &bytes[i], 1);
                        i++; p->position_in_message++;
                    }else{
                        error = true;
                        break;
                    }
                    if( error )
                        break;
                    if( i == count )
                        break;
                }
                break;
            case PARSER_STATE_LENGTH  :
                for(; p->state == PARSER_STATE_LENGTH ; ){
                    if( isdigit(bytes[i])  ){
                        p->msg_ptr->length = (p->msg_ptr->length * 10) + (bytes[i] - '0');
                        error = false;
                        p->state = PARSER_STATE_LENGTH;
                        sb_append(p->msg_ptr->msg_buf, &bytes[i], 1);
                        i++; p->position_in_message++;
                        
                    } else if( bytes[i] == '\n' ){
                        p->state = PARSER_STATE_BODY;
                        sb_append(p->msg_ptr->msg_buf, &bytes[i], 1);
                        p->msg_ptr->body_offset = (int)(p->position_in_message + 1L);
                        i++; p->position_in_message++;
                        error = false;
                    }else{
                        error = true;
                    }

                    if( error )
                        break;
                    if( i == count )
                        break;
                }
                
                break;
            case PARSER_STATE_BODY  :
                for(;;){
                    if( sb_get_used(p->msg_ptr->body) < p->msg_ptr->length  ){
                        sb_append(p->msg_ptr->msg_buf, &bytes[i], 1);
                        sb_append(p->msg_ptr->body, &bytes[i], 1);
                        
                        if(  sb_get_used(p->msg_ptr->body) == p->msg_ptr->length ){
                            done = true;
                            error = false;
                            i++; p->position_in_message++;
                            break;
                        }
                        error = false;
                        i++; p->position_in_message++;
                    }
                    if( error ) break;
                    if( i == count || done)
                        break;
                }
                break;
        }
        if( error ){
        
            p->cb(p, NULL, -1);
            bm_free(p->msg_ptr);
            i++; p->position_in_message++;
            bmp_init(p, p->cb);
        
        }else if( done ){
            
            p->cb(p, p->msg_ptr, 0);
            done = false;
            bmp_init(p, p->cb);
        
        }
        
    }
}
