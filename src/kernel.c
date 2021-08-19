#include <stddef.h>
#include <inttypes.h>

#include "console.h"
#include "memory.h"
#include "opensbi.h"

void kinit(uint64_t hartid, void* fdt) {
    for (int i = 0; i < 20; i++) {
        void* ptr = malloc(16);
        console_printf("allocation 0x%x: %p\n", i, ptr);
    }

    free_all();
    console_puts("Freed all memory!\n");

	while(1);
}

