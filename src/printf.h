#ifndef PRINTF_H
#define PRINTF_H
#include <stdarg.h>

__attribute__((format(printf, 2, 3)))
void func_printf(void (*write)(char), char* format, ...);
void func_vprintf(void (*write)(char), char* format, va_list va);

#endif /* PRINTF_H */
