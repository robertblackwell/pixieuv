#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

char* level_strings[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "VERBOSE"
};

void _sl_log(int lvl, int level, const char* func, int line, char* fmt, ... )
{
    if( lvl <= level ){
        char* prefix;
        char* lvl_str = level_strings[lvl - 1];
        int c = asprintf(&prefix, "%s: %s[%d] ", lvl_str, func, line);
//        int c = asprintf(&prefix, "%s: [%d] ", lvl_str,line);
        
        char* v_part;
        
        va_list arg_p;
        va_start(arg_p, fmt );
        int cc = vasprintf(&v_part, fmt, arg_p);
        
        fprintf(stderr, "%s%s", prefix, v_part);
//        printf("level : %d func: %s line: %d ", lvl, func, line);
//        vprintf(fmt, arg_p);
        va_end(arg_p);
        free(prefix);
        free(v_part);
    }
}
