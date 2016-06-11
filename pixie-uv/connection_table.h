/*
 * This is connection_table.h
 */
#ifndef blk_connection_table_h
#define blk_connection_table_h

#include <stdio.h>

struct connection_table_s
{
    blk_connection_t*   connections[1000];
    int                 count;
    int                 capacity;
};
typedef struct connection_table_s connection_table_t;

connection_table_t* connection_table_create();

void connection_table_free(connection_table_t* t);

int  connection_table_add(connection_table_t* t, void* entry);

void connection_table_remove(connection_table_t *t, void* entry);

#endif /* blk_connection_table_h */
