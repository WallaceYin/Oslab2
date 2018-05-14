#include <ylib.h>
#include <stdarg.h>
#define DEBUG

static char log_out[1024];
void Log(const char *fmt, ...);
void perror(const char *fmt);
