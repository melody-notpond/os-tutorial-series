#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

void console_puts(char*);
__attribute__((format(printf, 1, 2)))
void console_printf(char* format, ...);
void console_put_hexdump(void* data, size_t size);

#endif /* CONSOLE_H */
