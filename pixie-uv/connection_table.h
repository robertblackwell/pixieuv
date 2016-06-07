/*
 * This is connection_table.h
 */
#ifndef blk_connection_table_h
#define blk_connection_table_h

#include <stdio.h>

struct connection_table_s{
};
typedef struct connection_table_s connection_table_t;

connection_table_t* connection_table_create();

void connection_table_free(connection_table_t* t);

void connection_table_add(connection_table_t* t, void* entry);

void connection_table_remove(connection_table_t *t, void* entry);

#endif /* blk_connection_table_h */
