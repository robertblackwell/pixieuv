//
//  server.h
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef server_h
#define server_h

#include "connection_table.h"

struct server_s;
typedef struct server_s server_t;

typedef void (blk_handler_function)(blk_message_t* msg, blk_connection_t* conn);

struct server_s{
    uv_loop_t*              loop;
    blk_handler_function*   handler_function;
    connection_table_t*     connection_table;
    uv_tcp_t*               uv_tcp;
    
};

server_t*   server_create(uv_loop_t* loop, blk_handler_function handler);
void server_listen(server_t* server, int port);

#endif /* server_h */
