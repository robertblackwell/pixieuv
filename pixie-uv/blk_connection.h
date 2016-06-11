//
//  blk_connection.h
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef blk_connection_h
#define blk_connection_h

#include <stdio.h>
#include <uv.h>
#include "server.h"

struct blk_connection_s;
typedef struct blk_connection_s blk_connection_t;

//
// call back, internal to blk_connection, to signal that the write of a blk_message has completed
//
typedef void(blk_connection_on_write_msg_cb)(blk_connection_t* conn, blk_message_t* msg, int status);

//
// call back provided by server to be notified when conn is complete
//
typedef void(blk_connection_completion_cb)(server_t *server, blk_connection_t* conn, int status);

struct blk_connection_s{
    int                     connection_id;      //
    server_t*               server;             // the server object that started the connection
    
    uv_tcp_t*               stream;             // the active tcp connection
                                                // not owned by the connection do not dealloc
    
    uv_loop_t*              loop;               // active run loop
                                                // not owned by the connection do not dealloc
    
    blk_parser_t            parser;             // parse the input stream into blk_messages
    uv_write_t              write_req;          // for writing the output messsage
    
    //
    //  These next two ARE owned by the connection and should be cleaned up. BUT THEY
    //  ARE - connected. During and just after a write operation
    //
    //  (void*)current_message->msg_byf->buffer == buf.base
    //
    //   so becareful how this is cleaned up.
    //
    //  suggest using blk_connection_cleanup_write_buffer(conn)
    //
    blk_message_t*          current_message;    // current message being 'handled'
    uv_buf_t                buf;                // write buf temporary variable

    //
    // call back functions
    //
    blk_handler_function*               handler_function;   // called to process a message
    blk_connection_on_write_msg_cb*     write_message_cb;   // called when a message is written
    blk_connection_completion_cb*       completion_cb;      // called when connection is done
    
};

blk_connection_t* blk_connection_create(
                                        uv_tcp_t*       stream,
                                        uv_loop_t*      loop,
                                        server_t*       server,
                                        blk_handler_function* hf,
                                        blk_connection_completion_cb cb
                                        );

void blk_connection_set_id(blk_connection_t* conn, int id);

//
// Called to start reading data and operating the connection
//
void blk_connection_start(blk_connection_t* conn);

void blk_connection_read(blk_connection_t *conn);

void blk_connection_write(
                          blk_connection_t* conn,
                          blk_message_t* msg,
                          blk_connection_on_write_msg_cb* on_write_msg_cb
                          );

void blk_connection_free(blk_connection_t* conn);
#endif /* blk_connection_h */
