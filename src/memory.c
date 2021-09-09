#include <stdbool.h>

#include "memory.h"
#include "console.h"

extern int heap_bottom;
extern int pages_bottom;

struct malloc_header {
    void* next;
    size_t size;
};

struct {
    void* bump_top;

    struct malloc_header* bucket_16;
    struct malloc_header* bucket_32;
    struct malloc_header* bucket_64;
    struct malloc_header* bucket_128;
    struct malloc_header* bucket_256;
    struct malloc_header* bucket_512;
    struct malloc_header* bigger_than_512;
} global_allocator = { &heap_bottom, 0 };

#define MALLOC_HELPER(n)                                                        \
do {                                                                            \
    if (size <= n) {                                                            \
        if (!global_allocator.bucket_##n) {                                     \
            size_t bump_inc = sizeof(struct malloc_header) + n;                 \
            if (global_allocator.bump_top + bump_inc > (void*) &pages_bottom) { \
                console_puts("[malloc] Out of memory!\n");                      \
                return NULL;                                                    \
            }                                                                   \
                                                                                \
            struct malloc_header* header = global_allocator.bump_top;           \
            global_allocator.bump_top += bump_inc;                              \
            header->size = n;                                                   \
            return header + 1;                                                  \
        }                                                                       \
                                                                                \
        struct malloc_header* header = global_allocator.bucket_##n;             \
        global_allocator.bucket_##n = header->next;                             \
        return header + 1;                                                      \
    }                                                                           \
} while (0)

void* malloc(size_t size) {
    MALLOC_HELPER(16);
    MALLOC_HELPER(32);
    MALLOC_HELPER(64);
    MALLOC_HELPER(128);
    MALLOC_HELPER(256);
    MALLOC_HELPER(512);


    struct malloc_header* last = NULL;
    struct malloc_header* current = global_allocator.bigger_than_512;
    while (current) {
        if (current->size >= size) {
            if (current->size > size * 2 + sizeof(struct malloc_header)) {
                struct malloc_header* next = current->next;
                struct malloc_header* current_plus_1 = (void*) (current + 1) + 2 * size;
                current_plus_1->next = next;
                current_plus_1->size = current->size - size * 2 - sizeof(struct malloc_header);
                current->next = current_plus_1;
                current->size = size * 2;
            }

            if (last) {
                last->next = current->next;
            } else {
                global_allocator.bigger_than_512 = current->next;
            }

            return current + 1;
        }

        last = current;
        current = current->next;
    }

    size_t bump_inc = sizeof(struct malloc_header) + size;
    if (global_allocator.bump_top + bump_inc > (void*) &pages_bottom) {
        console_puts("[malloc] Out of memory!\n");
        return NULL;
    }

    struct malloc_header* header = global_allocator.bump_top;
    global_allocator.bump_top += bump_inc;
    header->size = size;
    return header + 1;
}

#undef MALLOC_HELPER

void free(void* ptr) {
    struct malloc_header* header = ptr;
    header--;

    switch (header->size) {
        case 16:
            header->next = global_allocator.bucket_16;
            global_allocator.bucket_16 = header;
            break;
        case 32:
            header->next = global_allocator.bucket_32;
            global_allocator.bucket_32 = header;
            break;
        case 64:
            header->next = global_allocator.bucket_64;
            global_allocator.bucket_64 = header;
            break;
        case 128:
            header->next = global_allocator.bucket_128;
            global_allocator.bucket_128 = header;
            break;
        case 256:
            header->next = global_allocator.bucket_256;
            global_allocator.bucket_256 = header;
            break;
        case 512:
            header->next = global_allocator.bucket_512;
            global_allocator.bucket_512 = header;
            break;
        default:
            if (header->size > 512) {
                header->next = global_allocator.bigger_than_512;
                global_allocator.bigger_than_512 = header;
            } else {
                console_printf("[free] Warning: attempted to free value likely not allocated by malloc: %p\n", ptr);
            }
            break;
    }
}
