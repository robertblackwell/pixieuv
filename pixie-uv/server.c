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

void on_connection_complete_cb(server_t* server, blk_connection_t* conn, int status)
{
    SLOGD("status %d \n");
}

void server_on_connection(uv_stream_t *server_stream, int status) {
    
    if (status < 0) {
        SLOGE(  "connection error %s\n", uv_strerror(status));
        return;
    }
    SLOGD( " status : %d \n", status);
    uv_loop_t*  loop = server_stream->loop;
    server_t* st = (server_t*)server_stream->data;
    blk_handler_function* hf = st->handler_function;
    
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    
    if (uv_accept(server_stream, (uv_stream_t*) client) == 0) {
        //
        // In here create the object that will server the client and initiate it
        //
        SLOGD("accept OK ");
        server_t* st = (server_t*)server_stream->data;
        blk_connection_t* new_conn = blk_connection_create(client, loop, hf);
        connection_table_add(st->connection_table, (void*)new_conn);
        blk_connection_start(new_conn, on_connection_complete_cb);
//        new_conn->handle = client;
//        new_conn->handler_function = ((server_t*)server_stream->data)->handler_function;
//        new_conn->loop = loop;
//        
//        
//        
//        echo_context_t * ec = ec_create();
//        ec->client = client;
//        client->data = (void*) ec;
//        
//        //        ec_handle(ec, some_callback_to_handle_service_complete);
//        uv_read_start((uv_stream_t*) client, cb_alloc_buffer, cb_read_in_echo);
    }
    else {
        SLOGE( "accept failed  \n");
        uv_close((uv_handle_t*) client, NULL);
    }
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
    int r = uv_listen((uv_stream_t*) server_tcp, 10, server_on_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return;
    }
    
}

