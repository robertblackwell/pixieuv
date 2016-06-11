//
//  connection_table.c
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "common.h"

SLOG_SETLEVEL(SLOG_LEVEL_WARN)

connection_table_t* connection_table_create()
{
    void* tmp = malloc(sizeof(connection_table_t));
//    memcpy(tmp, 0, sizeof(connection_table_t));
    connection_table_t* t = (connection_table_t*) tmp;
    memset(&(t->connections),0, 1000*sizeof(void*));
    t->capacity = 1000;
    t->count = 0;
    return t;
}

void connection_table_free(connection_table_t* t)
{
    
}

int connection_table_add(connection_table_t* t, void* entry)
{
    SLOGD("capacity : %d entry %lx \n", t->capacity, (long)entry);
    for(int i = 0; i < t->capacity; i++){
        if( t->connections[i] == NULL ){
            t->connections[i] = entry;
            return i;
        }
    }
    SLOGE("connection_table is FULL\n");
    exit(-1);
}

void connection_table_remove(connection_table_t *t, void* entry)
{
    for(int i = 0; i < t->capacity; i++){
        if( t->connections[i] == entry ){
            t->connections[i] = NULL;
            return;
        }
    }
    SLOGE("entry %lx not found in connection_table \n", (long) entry);
    exit(-1);
    
}
