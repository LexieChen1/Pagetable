# CS 3130 – Multi-Level Page Table Simulator

## Overview
This project simulates how an operating system translates virtual addresses to physical addresses using configurable **multi-level page tables**. As well as demonstrating the core concepts of virtual memory structure in modern operating systems.

## Building
```bash
make                  # Build libmlpt.a
make test_pagetable   # Build test executable
./test_pagetable      # Run tests
make clean            # Remove build files
```

---

## Configuration

### Customizing `config.h`
The `config.h` file defines constants that control your page table structure. Modify these values to experiment with different configurations:
```c
#define LEVELS 1   // Number of page-table levels
#define POBITS 12  // Number of bits for page offset
```

### Choosing Configuration Values

**`LEVELS`** (page table depth):
- `1` → Single-level table (simple, suitable for initial testing)
- `2` → Two-level table (typical for 32-bit systems)
- `3–4` → Multi-level tables (for larger 64-bit address spaces)

**`POBITS`** (page size):
- `10` → 1 KB pages (1,024 bytes)
- `12` → 4 KB pages (4,096 bytes) — **standard size**
- `13` → 8 KB pages (8,192 bytes)
- `14+` → 16 KB or larger pages

---

## Usage Example
```c
#include "mlpt.h"
#include 

int main() {
    // Allocate a page starting at virtual address 0x1000
    if (allocate_page(0x1000) == 0) {
        printf("Page allocated successfully\n");
        
        // Translate virtual address to physical address
        size_t physical_addr = translate(0x1000);
        
        if (physical_addr != ~0UL) {
            printf("Virtual 0x1000 → Physical 0x%lx\n", physical_addr);
        } else {
            printf("Translation failed\n");
        }
    }
    
    return 0;
}
```

---

## Known Limitations
1. **Architecture dependency**: Assumes 64-bit architecture (`sizeof(size_t) == 8`)
2. **Simulated memory**: Uses `posix_memalign()` to simulate page allocation rather than real hardware page frames
3. **No page deallocation**: Currently does not implement freeing allocated pages

---