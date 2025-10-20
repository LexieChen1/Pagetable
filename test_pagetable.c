#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlpt.h"
#include "config.h"

static int alloc_count = 0;

// redefine posix_memalign (link-time override)
int posix_memalign(void **memptr, size_t alignment, size_t size) {
    alloc_count++;
    void *ptr;
    int result = aligned_alloc(alignment, size) ? 0 : 1; // fallback
    *memptr = aligned_alloc(alignment, size);
    return 0;
}

// helper function to print PTE
void print_pte(int index) {
    size_t *table = (size_t *)ptbr;
    size_t entry = table[index];
    printf("PTE[%d]: valid=%d  PPN=0x%lx\n",
           index,
           (int)(entry & 1),
           (long)(entry >> POBITS));
}

int main() {
    size_t va1 = 0x1000;     // page 1
    size_t va2 = 0x2000;     // page 2
    size_t va3 = 0x1000;     // same as va1 (should not reallocate)
    
    printf("Initial ptbr: %zu\n", ptbr);

    // First call — should allocate 2 pages (root + data)
    alloc_count = 0;
    int r1 = allocate_page(va1);
    printf("allocate_page(0x%zx) → %d, allocations: %d\n", va1, r1, alloc_count);
    print_pte(1);

    // Second call — new page number, expect 1 allocation
    alloc_count = 0;
    int r2 = allocate_page(va2);
    printf("allocate_page(0x%zx) → %d, allocations: %d\n", va2, r2, alloc_count);
    print_pte(2);

    // Third call — same as first page, expect 0 allocations
    alloc_count = 0;
    int r3 = allocate_page(va3);
    printf("allocate_page(0x%zx) → %d, allocations: %d\n", va3, r3, alloc_count);
    print_pte(1);

    return 0;
}