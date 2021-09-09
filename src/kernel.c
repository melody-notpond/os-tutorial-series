#include <stddef.h>
#include <inttypes.h>

#include "console.h"
#include "memory.h"
#include "opensbi.h"

void kinit(uint64_t hartid, void* fdt) {
    int sizes[] = { 5, 16, 31, 42, 69, 131, 300, 500, 600};
    for (int i = 0; i < sizeof(sizes) / sizeof(typeof(*sizes)); i++) {
        void* ptr = malloc(sizes[i]);
        console_printf("allocation 0x%x with size 0x%x: %p\n", i, sizes[i], ptr);
        free(ptr);
    }

    console_puts("Freed all memory!\n");
	while(1);
}
