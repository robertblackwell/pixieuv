#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <uv.h>
#include <assert.h>

#include "common.h"

SLOG_SETLEVEL(SLOG_LEVEL_DEBUG)

#define DEFAULT_PORT 8001
#define DEFAULT_BACKLOG 10


#ifdef NOTUSED
uv_loop_t* loop;

void cb_alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    
    uv_buf_t tmp = uv_buf_init((char*) malloc(suggested_size), (int)suggested_size);
    void* p = (void*) tmp.base;
    *buf = tmp;
    
    SLOGD("cb_alloc_buffer buf.base : %lx len: %d \n", (long)(void*)tmp.base, (int)tmp.len);
}
typedef struct echo_context_s{
    uv_tcp_t*   client;
    uv_write_t*	req;
    uv_buf_t 	uv_buf;
    void*		data;
} echo_context_t;

echo_context_t* ec_create(){
    return (echo_context_t*) malloc(sizeof(echo_context_t));
}

static void echo_write_data(uv_stream_t *dest, size_t size, uv_buf_t* buf, uv_write_cb cb) {
    echo_context_t* ec = (echo_context_t*) dest->data;

    SLOGD( "echo_write_data size: %ld\n", size);
    
    uv_write_t *req = (uv_write_t*) malloc(sizeof(uv_write_t));
    ec->req = req;
    req->data = (void*) ec;

//    buf->len = 3;
    uv_write((uv_write_t*) req, (uv_stream_t*)dest, buf, 1, cb);
}
void cb_on_write(uv_write_t *req, int status) {
    SLOGD( "cb_on_write\n");
    // should do dome type of cleanup - maybe reset for next message
}
void cb_read_in_echo(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {

    echo_context_t* ec = (echo_context_t*) stream->data;
    
    SLOGD( "read_in_echo got data nread: %d \n", (int)nread);
    if (nread < 0){
        if (nread == UV_EOF){
            SLOGD( "got close from client\n");
            // fanout_close(t);
            uv_close( (uv_handle_t*) stream, NULL);
        }
    } else if (nread > 0) {

        SLOGD( "read_in_echo got data buf : %lx nread: %d buf->len : %d\n", (long)buf, (int)nread, (int)buf->len);
        SLOGD( "read_in_echo got data %.*s \n", (int)nread, buf->base);

        // remove the const from argument
        uv_buf_t* tmp_buf = (uv_buf_t*)buf;
        tmp_buf->len = nread;
        ec->uv_buf = *tmp_buf;
        echo_write_data(stream, nread, tmp_buf, cb_on_write);
        // }
    }
    
    if (buf->base)
        free(buf->base);
}
/*
 * on_new_connection. 
 *
 *  Complete the process of accept - ing the new client connection. If this does not fail
 *  setup the connection object and start the process of communicating with the client.
 */
void on_new_connection(uv_stream_t *server, int status) {
    
    if (status < 0) {
        SLOGE(  "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }
    SLOGD( "new connections status : %d \n", status);
    
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        //
        // In here create the object that will server the client and initiate it
        //
        SLOGD("accept OK ");
        echo_context_t * ec = ec_create();
        ec->client = client;
        client->data = (void*) ec;
        
//        ec_handle(ec, some_callback_to_handle_service_complete);
        uv_read_start((uv_stream_t*) client, cb_alloc_buffer, cb_read_in_echo);
    }
    else {
        SLOGE( "accept failed  \n");
        uv_close((uv_handle_t*) client, NULL);
    }
}
#endif


void echo_handler_function(blk_message_t* msg, blk_connection_t* conn){
    SLOGD(" got here");
}

int main() {
    uv_loop_t* loop;
    loop = uv_default_loop();
    SLOGI("We are up and running \n");

    server_t* server = server_create(loop, echo_handler_function);
    server_listen(server, 8001);
    
    return uv_run(loop, UV_RUN_DEFAULT);
}

