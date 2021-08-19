#include <stdarg.h>

#include "console.h"
#include "opensbi.h"
#include "printf.h"

void console_puts(char* s) {
    while (*s) {
        sbi_console_putchar(*s);
        s++;
    }
}

__attribute__((format(printf, 1, 2)))
void console_printf(char* format, ...) {
    va_list va;
    va_start(va, format);
    func_vprintf(sbi_console_putchar, format, va);
    va_end(va);
}

unsigned int console_log_16(size_t n) {
    unsigned int i = 0;
    while (n) {
        n >>= 4;
        i++;
    }
    return i;
}

void console_put_hexdump(void* data, size_t size) {
    unsigned int num_zeros = console_log_16(size);
    unsigned char* data_char = (unsigned char*) data;

    for (size_t i = 0; i < (size + 15) / 16; i++) {
        // Print out buffer zeroes
        unsigned int num_zeros_two = num_zeros - console_log_16(i) - 1;
        for (unsigned int j = 0; j < num_zeros_two; j++) {
            console_printf("%x", 0);
        }

        // Print out label
        console_printf("%lx    ", i * 16);

        // Print out values
        for (int j = 0; j < 16; j++) {
            size_t index = i * 16 + j;

            // Skip values if the index is greater than the number of values to dump
            if (index >= size)
                console_puts("   ");
            else {
                // Print out the value
                if (data_char[index] < 16)
                    console_printf("%x", 0);
                console_printf("%x ", data_char[index]);
            }
        }

        console_puts("    |");

        // Print out characters
        for (int j = 0; j < 16; j++) {
            size_t index = i * 16 + j;

            // Skip characters if the index is greater than the number of characters to dump
            if (index >= size)
                sbi_console_putchar('.');

            // Print out printable characters
            else if (32 <= data_char[index] && data_char[index] < 127)
                sbi_console_putchar(data_char[index]);

            // Nonprintable characters are represented by a period (.)
            else
                sbi_console_putchar('.');
        }

        console_puts("|\n");
    }
}
