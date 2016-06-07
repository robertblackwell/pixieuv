
#ifdef NEW__LOG
#define NONE    0x00
#define DBG     0x1F
#define INFO    0x0F
#define ERR     0x07
#define EMR     0x03
#define CRIT    0x01

#define DEBUG_LEVEL ERR

#define WHERESTR "[FILE : %s, FUNC : %s, LINE : %d]: "
#define WHEREARG __FILE__,__func__,__LINE__
#define RBDEBUG(...)  fprintf(stderr, __VA_ARGS__)
#define RBDEBUG_PRINT(X, _fmt, ...)  if((DEBUG_LEVEL & X) == X) \
RBDEBUG(WHERESTR _fmt, WHEREARG,__VA_ARGS__)
#endif



#define SLMACRO(lvl, frmt, ...) \
_sl_log( lvl, slog_level, __func__, __LINE__, frmt, ##__VA_ARGS__)


#define SLOG_LEVEL_ERROR     1
#define SLOG_LEVEL_WARN      2
#define SLOG_LEVEL_INFO      3
#define SLOG_LEVEL_DEBUG     4
#define SLOG_LEVEL_VERBOSE   5


#define  SLOGError(frmt, ...)   SLMACRO(SLOG_LEVEL_ERROR, frmt, ##__VA_ARGS__)
#define  SLOGWarn(frmt, ...)    SLMACRO(SLOG_LEVEL_WARN, frmt, ##__VA_ARGS__)
#define  SLOGInfo(frmt, ...)    SLMACRO(SLOG_LEVEL_INFO, frmt, ##__VA_ARGS__)
#define  SLOGDebug(frmt, ...)   SLMACRO(SLOG_LEVEL_DEBUG, frmt, ##__VA_ARGS__)
#define  SLOGVerbose(frmt, ...) SLMACRO(SLOG_LEVEL_VERBOSE, frmt, ##__VA_ARGS__)

#define  SLOGE(frmt, ...)  SLMACRO(SLOG_LEVEL_ERROR, frmt, ##__VA_ARGS__)
#define  SLOGW(frmt, ...)  SLMACRO(SLOG_LEVEL_WARN, frmt, ##__VA_ARGS__)
#define  SLOGI(frmt, ...)  SLMACRO(SLOG_LEVEL_INFO, frmt, ##__VA_ARGS__)
#define  SLOGD(frmt, ...)  SLMACRO(SLOG_LEVEL_DEBUG, frmt, ##__VA_ARGS__)
#define  SLOGV(frmt, ...)  SLMACRO(SLOG_LEVEL_VERBOSE, frmt, ##__VA_ARGS__)


#define SLOG_SETLEVEL(level) static int slog_level = level;

void _sl_log(int lvl, int level, const char *func, int line, char* fmt, ... );

