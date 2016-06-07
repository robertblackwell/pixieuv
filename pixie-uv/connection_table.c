//
//  connection_table.c
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "common.h"

connection_table_t* connection_table_create()
{
    void* tmp = malloc(sizeof(struct connection_table_s));
    connection_table_t* t = (connection_table_t*) tmp;
    
    return t;
}

void connection_table_free(connection_table_t* t)
{
    
}

void connection_table_add(connection_table_t* t, void* entry)
{
    
}

void connection_table_remove(connection_table_t *t, void* entry)
{
    
}
