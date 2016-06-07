//
//  blk_connection.c
//  pixie-uv
//
//  Created by ROBERT BLACKWELL on 6/6/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <assert.h>
#include "common.h"

SLOG_SETLEVEL(SLOG_LEVEL_DEBUG)

static void cb_alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    uv_buf_t tmp = uv_buf_init((char*) malloc(suggested_size), (int)suggested_size);
    void* p = (void*) tmp.base;
    *buf = tmp;
    
    SLOGD("cb_alloc_buffer buf.base : %lx len: %d \n", (long)(void*)tmp.base, (int)tmp.len);
}

void on_write_msg(blk_connection_t* conn, blk_message_t* msg, int status)
{
    SLOGD("seems like write completed");
}

void parser_cb(blk_parser_t* parser, blk_message_t* msg, int status)
{
    if( status == 0 ){
        SLOGD("parsing completed OK");
        blk_connection_t* conn = (blk_connection_t*)parser->data;
        blk_connection_write(conn, msg, on_write_msg);
    }else{
        
    }
}

void cb_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    
    blk_connection_t* conn = (blk_connection_t*) stream->data;
    
    SLOGD( "read_in_echo got data nread: %d \n", (int)nread);
    if (nread < 0){
        if (nread == UV_EOF){
            SLOGD( "got close from client\n");
            // fanout_close(t);
            uv_close( (uv_handle_t*) stream, NULL);
        }
    } else if (nread > 0) {
        SLOGD("go some input bytes - now parse them");
        uv_buf_t* tmp_buf = (uv_buf_t*)buf;
        tmp_buf->len = nread;
        bmp_append(&(conn->parser), tmp_buf->base, (int)nread);
    }
}

blk_connection_t* blk_connection_create(
                                        uv_tcp_t* stream,
                                        uv_loop_t* loop,
                                        server_t* server,
                                        blk_handler_function* hf,
                                        blk_connection_completion_cb completion_cb
                                       )
{
    void* tmp = malloc(sizeof(blk_connection_t));
    blk_connection_t* t = (blk_connection_t*)tmp;
    t->stream = stream;
    t->loop = loop;
    t->server = server;
    t->handler_function = hf;
    t->completion_cb = completion_cb;
    bmp_init(&(t->parser), parser_cb);
    t->parser.data = (void*)t;
    t->current_message = NULL;
    t->write_req = NULL;
    return t;
}
void blk_connection_read(blk_connection_t *conn, void(on_blk_message)(blk_message_t msg, int status))
{
    uv_stream_t* stream;
    stream = (uv_stream_t*)conn->stream;
    SLOGD("start a message read");
    uv_read_start((uv_stream_t*) stream, cb_alloc_buffer, cb_read);

}

void on_write_bytes_cb(uv_write_t *req, int status)
{
    blk_connection_t* conn = (blk_connection_t*) req->data;
    blk_message_t* msg = conn->current_message;
    if( status == 0 ){
        SLOGD("write completed");
        (*conn->write_message_cb)(conn, msg, status);
    }else{
        
    }
}

void blk_connection_write(
                  blk_connection_t* conn,
                  blk_message_t* msg,
                  blk_connection_on_write_msg_cb* on_write_msg_cb
    )
{
    char*       base = msg->msg_buf->buffer;
    long        size = msg->msg_buf->used;
    
    uv_buf_t    buf = uv_buf_init(base, (uint)size);
    uv_tcp_t*   dest = conn->stream;
    
    uv_write_t *req = (uv_write_t*) malloc(sizeof(uv_write_t));
    conn->write_req = req;
    conn->current_message = msg;
    req->data = (void*) conn;
    conn->write_message_cb = on_write_msg_cb;
    SLOGD("write a message to client");
    uv_write((uv_write_t*) req, (uv_stream_t*)dest, &buf, 1, on_write_bytes_cb);

}