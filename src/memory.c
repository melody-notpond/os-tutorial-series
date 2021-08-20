#include <stdbool.h>

#include "memory.h"
#include "console.h"

extern int heap_bottom;
extern int pages_bottom;

struct malloc_header {
    void* next;
    size_t size;
};

bool initialised = false;
struct malloc_header* free_list = NULL;

void* malloc(size_t size) {
    if (!initialised) {
        free_list = (struct malloc_header*) &heap_bottom;
        *free_list = (struct malloc_header) {
            .next = NULL,
            .size = &pages_bottom - &heap_bottom - sizeof(struct malloc_header)
        };
        initialised = true;
    }

    struct malloc_header* last = NULL;
    struct malloc_header* current = free_list;
    while (current) {
        if (current->size >= size) {
            if (current->size > size * 2 + sizeof(struct malloc_header)) {
                struct malloc_header* next = current->next;
                struct malloc_header* current_plus_1 = (void*) (current + 1) + 2 * size;
                current_plus_1->next = next;
                current_plus_1->size = current->size - size * 2 - sizeof(struct malloc_header);
                current->size = size * 2;
                current->next = current_plus_1;
            }

            if (last) {
                last->next = current->next;
            } else {
                free_list = current->next;
            }

            return current + 1;
        }

        last = current;
        current = current->next;
    }

    console_puts("[malloc] Out of memory!\n");
    return NULL;
}

void free(void* ptr) {
    struct malloc_header* header = ptr;
    header--;
    header->next = free_list;
    free_list = header;
}
