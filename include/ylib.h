/* ylib is a C library named by Wallace Yin.
*/

#include <am.h>
#include <amdev.h>
#include <stdarg.h>

//string.h
void *memset(void *arr, int val, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcmp(const char *str_1, const char *str_2);
char *strstr(const char *str1, const char *str2);
void itoa(int dex, char *dest);

//stdlib.h
void srand(unsigned int seed);
int rand(void);

//stdio.h
int printf(const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list ap);
//int sprintf(char *dest, const char *fmt, ...);
//int snprintf(char *dest, size_t n, const char *fmt, ...);

//assert.h
//FIXME: assert is not completed.
