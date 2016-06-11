//
//  server.c
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <uv.h>
#include <logger/logger.h>
#include "common.h"
SLOG_SETLEVEL(SLOG_LEVEL_DEBUG)

#define DEFAULT_PORT 8001

//
// Forward declares
//
static void on_connection_complete_cb(server_t* server, blk_connection_t* conn, int status);
void on_uv_client_close(uv_handle_t* client);

//
//  callback for new connection.
//  create a new connection object and initialize
//  setup completion call back
//  set up the handler function to process the input message and create the output message
//  add it to the connection table
//  start the connection
//
static void on_connection_new( uv_stream_t *server_stream, int status )
{
    
    if (status < 0)
    {
        SLOGE(  "connection error %s\n", uv_strerror(status));
    }
    else
    {
        uv_loop_t*  loop = server_stream->loop;
        server_t* server = (server_t*)(server_stream->data);
        blk_handler_function* hf = server->handler_function;
        
        uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
        uv_tcp_init(loop, client);
        
        if (uv_accept(server_stream, (uv_stream_t*) client) == 0)
        {
            blk_connection_t* new_conn = blk_connection_create(client, loop, server, hf, on_connection_complete_cb);
            int id = connection_table_add(server->connection_table, (void*)new_conn);
            SLOGD("accept OK id : %d client: %lx \n", id, (long)client);
            blk_connection_set_id(new_conn, id);
            blk_connection_start(new_conn);
        }
        else
        {
            SLOGE( "accept failed  \n");
            uv_close((uv_handle_t*) client, NULL);
        }
    }
}
//
//  completion callback
//  remove connection from connection table
//  free the connection object
//
static void on_connection_complete_cb(server_t* server, blk_connection_t* conn, int status)
{
    SLOGD("status %d id: %d stream : %lx \n", status, conn->connection_id, conn->stream);
    connection_table_t* ct = server->connection_table;
    connection_table_remove(ct, (void*) conn);
    uv_tcp_t* client = conn->stream;
    uv_close((uv_handle_t*)client, on_uv_client_close);
    blk_connection_free(conn);
}
void on_uv_client_close(uv_handle_t* client){
    SLOGV("%lx \n", (long)client);
    free(client);
}



server_t*   server_create(uv_loop_t* loop, blk_handler_function handler)
{
    void* tmp = malloc(sizeof(struct server_s));
    server_t* t = (server_t*) tmp;
    t->loop = loop;
    t->handler_function = handler;
    t->connection_table = connection_table_create();
    return t;
}

void server_listen(server_t* server, int port)
{
    uv_loop_t*          loop = server->loop;
    struct sockaddr_in  addr;
    uv_tcp_t*           server_tcp = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, server_tcp);
    server->uv_tcp = server_tcp;
    server_tcp->data =   (void*) server;
    
    
    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
    
    uv_tcp_bind(server_tcp, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) server_tcp, SOMAXCONN, on_connection_new);
    if (r) {
        SLOGE("Listen error %s\n", uv_strerror(r));
        return;
    }
    
}

