#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

char* level_strings[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "VERBOSE"
};

void _sl_log(int lvl, int level, const char* func, int line, char* fmt, ... )
{
    if( lvl <= level )
    {
        char* prefix;
        char* lvl_str = level_strings[lvl - 1];
        asprintf(&prefix, "%s: %s[%d]\t\t ", lvl_str, func, line);
        
        char* v_part;
        
        va_list arg_p;
        va_start(arg_p, fmt );
        
        vasprintf(&v_part, fmt, arg_p);
        
        fprintf(stderr, "%s%s", prefix, v_part);
        va_end(arg_p);
        free(prefix);
        free(v_part);
    }
}
