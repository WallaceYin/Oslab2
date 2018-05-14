#include <debug.h>

static char log_out[1024];
void Log(const char *fmt, ...) {
	printf("\033[34m");
	memset(log_out, 0, 1024);
	va_list ap;
	va_start(ap, fmt);
	vsprintf(log_out, fmt, ap);
	for (int i = 0; i < strlen(log_out); i++)
		_putc(log_out[i]);
	va_end(ap);
	printf("\n");
	printf("\033[0m");
}

void perror(const char *fmt) {
	printf("\033[31m");
	for (char *p = (char *)fmt; *p; p++)
		_putc(*p);
	printf("\n");
	printf("\033[0m");
}
