//
//  simple_buffer.m
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/17/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "simple_buffer.h"

void sb_die(char* message){
    fprintf(stderr, "%s\n", message);
    exit(-1);
}

void
sb_dump(simple_buffer_t* sb, char* message)
{
    //NSLog(@"SB Dump  %@ SB: %x buf: %x used: %d length: %d", message, (int)sb, (int)sb->buffer, sb->used, sb->length);
}

bool xstrncmp(char* one, char* two, int n){
    for(int i=0; i < n; i++){
        if( one[i] != two[i])
            return false;
    }
    return true;
}

void sb_test_guards(simple_buffer_t* sb, char* msg)
{
    return;
    char* opn = "OPN";
    char* cls = "CLS";
    if( xstrncmp(sb->open_guard, opn, 3) && xstrncmp(sb->close_guard, cls, 3) )
        return;
    sb_die("simple_buffer sb_test_guard failed ");
 
}

simple_buffer_t* sb_create()
{
    simple_buffer_t* sb = malloc(sizeof(simple_buffer_t));
    strcpy(sb->open_guard, "OPN");
    strcpy(sb->close_guard, "CLS");
    sb->length = kSIMPLE_BUFFER_SIZE;
    sb->used = 0;
    sb->buffer = malloc(kSIMPLE_BUFFER_SIZE);
    sb_test_guards(sb, "SB New");
    sb_dump(sb, "SBNew");
    return sb;
}

int sb_get_used(simple_buffer_t* sb){
    return sb->used;
}

void sb_free(simple_buffer_t* sb)
{
    sb_test_guards(sb, "Free");
    sb_dump(sb, " SBFREE");
    free(sb->buffer);
    free(sb);
}
void sb_append(simple_buffer_t* sb, char* at, size_t length )
{    
    //sb_test_guards(sb, "Append begin");
    //sb_dump(sb, "Append ");
    
    if(sb->used + length > sb->length){
        // extend buffer
        unsigned long min_space_needed = length + sb->used;// - sb->length;
        
        char* b = malloc(sb->length + (
                                       (kSIMPLE_BUFFER_SIZE >= min_space_needed)
                                            ? kSIMPLE_BUFFER_SIZE
                                            :  min_space_needed
                                       )
                         );
        memcpy(b, &(sb->buffer[0]), sb->used);
        char* tmp = sb->buffer;
        free(tmp);
        sb->buffer = b;
    }
    if( length == 1 ){
        sb->buffer[sb->used] = at[0];
    }else{
        memcpy(&sb->buffer[sb->used], at, length );
    }
    sb->used += length;
    sb_test_guards(sb, "Append end");
    sb_dump(sb, "After append");
}

int sb_to_string(simple_buffer_t* sb, char** buf)
{
    if( sb == NULL )
        return asprintf(buf, "SimpleBuffer is NULL ");
    else
        return asprintf(buf, "SimpleBuffer lengthL %d used: %d buf: %.*s", sb->length, sb->used, sb->used, sb->buffer );
}

