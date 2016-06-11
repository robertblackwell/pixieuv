
#include <assert.h>
#include "common.h"

SLOG_SETLEVEL(SLOG_LEVEL_WARN)

/*
 * -------------------------------------------------------------------------------
 *  Methods for managing write buffers within a blk_connection_t
 * -------------------------------------------------------------------------------
 */
#pragma mark - write buffer management methods
//
// Cleans up both current_message and buf fields in a manner that
// understands the conenection between these two fields
//
static void blk_connection_cleanup_write_buffer(blk_connection_t* conn)
{
    SLOGD("%lx\n", (conn->buf.base));
    if( conn->current_message != NULL ){
        bm_free(conn->current_message );
        conn->current_message = NULL;
        conn->buf.base = NULL;
        return;
    }
    assert(conn->current_message == NULL && conn->buf.base == NULL);
}


//
// specifically cealns up the buf (by value) field in blk_connection_t
//
//  SHOULD NOT BE USED
//
static void blk_connection_clear_buf(blk_connection_t* conn)
{
    if( conn->buf.base != NULL ){
        free(conn->buf.base);
        conn->buf.base = NULL;
    }
}


//
// Two be used on uv_buf_t that ARE NOT conn->buf - mainy on_read functions
//
void uv_buf_cleanup(uv_buf_t* b)
{
    SLOGD("%lx\n", b->base);
    if( b->base != NULL ) {
        free(b->base);
        b->base = NULL;
    }
}

#pragma mark - get/set functions for context - gets blk_connection_t* from 'data' fields in uv_TYPE_t and blk_parser_t
/*
 * -------------------------------------------------------------------------------------------------
 * functions for getting/setting blk_connection_t* from context fields in uv_TYPE_t and blk_parser_t
 * -------------------------------------------------------------------------------------------------
 */
blk_connection_t* uv_stream_get_conn(uv_stream_t* stream)
{
    return  (blk_connection_t*) stream->data;

}
void uv_stream_set_conn(uv_stream_t* stream, blk_connection_t* conn)
{
    stream->data = (void*) conn;
}

blk_connection_t* uv_write_get_conn(uv_write_t* req)
{
    return  (blk_connection_t*) req->data;
    
}

void uv_write_set_conn(uv_write_t* req, blk_connection_t* conn)
{
    req->data = (void*) conn;
}

blk_connection_t* blk_parser_get_conn(blk_parser_t* parser)
{
    return  (blk_connection_t*) parser->data;
    
}
void blk_parser_set_conn(blk_parser_t* parser, blk_connection_t* conn)
{
    parser->data = (void*) conn;
}

#pragma mark - callback functions, forward declared
/*
 * -------------------------------------------------------------------------------------------------
 * callback functions used by blk_connection_t objects - all forward declared
 * -------------------------------------------------------------------------------------------------
 */

static void cb_alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

static void on_uv_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

static void on_parsed_msg(blk_parser_t* parser, blk_message_t* msg, int status);

static void on_uv_write(uv_write_t *req, int status);

static void on_msg_write(blk_connection_t* conn, blk_message_t* msg, int status);

static void on_eof(blk_connection_t* conn);

static void on_error(blk_connection_t* conn);

//
// called by libuv to get a new buffer
//
static void cb_alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    suggested_size = 1000;
    uv_buf_t tmp = uv_buf_init((char*) malloc(suggested_size), (int)suggested_size);
    *buf = tmp;
    
    SLOGD("cb_alloc_buffer buf.base : %lx len: %d \n", (long)(void*)tmp.base, (int)tmp.len);
}

//
// called when some data is available - push that data into the message parser
//
static void on_uv_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    
    blk_connection_t* conn = (blk_connection_t*) stream->data;
    
    SLOGD( "read_in_echo got data nread: %d \n", (int)nread);
    if (nread < 0){
        uv_buf_cleanup((uv_buf_t*)buf);
        if (nread == UV_EOF){
            SLOGD( "got close from client\n");
            // fanout_close(t);
            on_eof(conn);
        }else{
            on_error(conn);
        }
    } else if (nread > 0) {
        SLOGD("go some input bytes - now parse them\n");
        uv_buf_t* tmp_buf = (uv_buf_t*)buf;
        tmp_buf->len = nread;
        bmp_append(&(conn->parser), tmp_buf->base, (int)nread);
        uv_buf_cleanup(tmp_buf);
    }
}
//
// called when a complete message has been parsed from the input stream
//
static void on_parsed_msg(blk_parser_t* parser, blk_message_t* msg, int status)
{
    blk_connection_t* conn = (blk_connection_t*)parser->data;
    if( status == 0 ){
//        SLOGW("msg to process then write id: %d \n", conn->connection_id);
        printf(".\n");
        //
        // In here should call the handler function so that it can do something with the input message
        // and generate an output message
        //
        blk_connection_write(conn, msg, on_msg_write);
    }else{
        on_error(conn);
    }
}

//
// called when a uv_write of some bytes is complete
//
static void on_uv_write(uv_write_t *req, int status)
{
    blk_connection_t* conn = (blk_connection_t*) req->data;
    blk_message_t* msg = conn->current_message;
    if( status == 0 ){
        SLOGD("write completed\n");
        bm_free(conn->current_message);
        //
        // These next two lines are a bit ricky
        //
        conn->current_message = NULL;
        conn->buf.base = NULL;
        
        (*conn->write_message_cb)(conn, msg, status);
    }else{
        on_error(conn);
    }
}

//
// called when a reply message has been written
//
static void on_msg_write(blk_connection_t* conn, blk_message_t* msg, int status)
{
    SLOGD("seems like write completed\n");
}

static void on_eof(blk_connection_t* conn)
{
    SLOGD("EOF id: %d \n", conn->connection_id);
    server_t* server = conn->server;
    blk_connection_completion_cb* cb = conn->completion_cb;
    cb(server, conn, 0);
}

static void on_error(blk_connection_t* conn)
{
    SLOGE("ERROR id: %d ", conn->connection_id);
    server_t* server = conn->server;
    blk_connection_completion_cb* cb = conn->completion_cb;
    cb(server, conn, -1);
    
}

#pragma mark - public methods
/*
 * -------------------------------------------------------------------------------------------------
 * public methods
 * -------------------------------------------------------------------------------------------------
 */
//
// create a new blk_connection object and hook it up to other objects
//
blk_connection_t* blk_connection_create(
                                        uv_tcp_t*           stream,
                                        uv_loop_t*          loop,
                                        server_t*           server,
                                        blk_handler_function* hf,
                                        blk_connection_completion_cb completion_cb
                                       )
{
    void* tmp = malloc(sizeof(blk_connection_t));
    SLOGD("%lx\n", (long)tmp);
    blk_connection_t* t = (blk_connection_t*)tmp;
    t->stream           = stream;
    t->stream->data     = (void*) t;
    t->loop             = loop;
    t->server           = server;
    t->buf              = uv_buf_init(NULL, 0);
    t->handler_function = hf;
    t->completion_cb    = completion_cb;
    bmp_init(&(t->parser), on_parsed_msg);
    t->parser.data      = (void*)t;
    t->current_message  = NULL;
    memset(&(t->write_req), 0, sizeof(uv_write_t));
    return t;
}
void blk_connection_set_id(blk_connection_t* conn, int id)
{
    conn->connection_id = id;
}


void blk_connection_start(blk_connection_t* conn)
{
    blk_connection_read(conn);
}

void blk_connection_read(blk_connection_t *conn)
{
    uv_stream_t* stream;
    stream = (uv_stream_t*)conn->stream;
    SLOGD("start a message read id: %d \n", conn->connection_id);
    uv_read_start((uv_stream_t*) stream, cb_alloc_buffer, on_uv_read);

}


void blk_connection_write(
                  blk_connection_t* conn,
                  blk_message_t* msg,
                  blk_connection_on_write_msg_cb* on_write_msg_cb
    )
{
    //
    // close relationship between conn->current_message and conn->buf
    //
    conn->current_message   = msg;
    char*    base           = msg->msg_buf->buffer;
    long     size           = msg->msg_buf->used;
    conn->buf  = uv_buf_init(base, (uint)size);
    
    uv_tcp_t*   dest    = conn->stream;
    
    //
    // This is either a memory leak or a bug - use the request inside conn
    //
    uv_write_t  *req    = &(conn->write_req);
    memset(&(conn->write_req), 0, sizeof(uv_write_t));

    req->data = (void*) conn;
    conn->write_message_cb = on_msg_write;
    SLOGD("write a message to client\n");
    uv_write((uv_write_t*) req, (uv_stream_t*)dest, &(conn->buf), 1, on_uv_write);

}

void blk_connection_free(blk_connection_t* conn)
{
    if( conn->current_message != NULL ){
        bm_free(conn->current_message);
    }
    bmp_deinit(conn->parser);
    blk_connection_cleanup_write_buffer(conn);
    free(conn);
}