#include <stdio.h>
#include <string.h>
#include "blk_message.h"
#include "blk_parser.h"
#include <stdarg.h>
#include <logger/logger.h>

SLOG_SETLEVEL(SLOG_LEVEL_DEBUG)

void parser_cb(blk_message_t* msg, int status){
    if( status == 0 ){
        char* ss;
        int c = sb_to_string(msg->msg_buf, &ss);
        SLOGD("CB msg: %s status: %d\n", ss, status);
    }else{
        SLOGD("CB status: %d\n", status);
    }
}


int main(int argc, const char * argv[])
{
//    SLMACRO(SLOG_LEVEL_INFO, "this is a frmt %d \n", 33);
//    
//    SLOGError("this is an error message code : %d\n", -22);
//    SLOGW("this is an warn message code : %d\n", -22);
//    SLOGI("this is an info message code : %d\n", -22);
//    SLOGD("this is an debug message code : %d\n", -22);
//    SLOGV("this is an verbose message code : %d\n", -22);
    
    char* msg = "START\n02\n01xxSTART\n10\n0123456789";
    blk_parser_t bmp;
    bmp_init(&bmp, parser_cb);
    bmp_append(&bmp, msg, strlen(msg));
    
    
    return 0;
}
