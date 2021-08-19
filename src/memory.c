#include "memory.h"
#include "console.h"

extern int heap_bottom;
extern int pages_bottom;

void* bump_top = &heap_bottom;

void* malloc(size_t size) {
    if (bump_top + size > (void*) &pages_bottom) {
        console_puts("[malloc] out of memory!\n");
        return NULL;
    }

    void* ptr = bump_top;
    bump_top += size;
    return ptr;
}

void free_all() {
    bump_top = &heap_bottom;
}
