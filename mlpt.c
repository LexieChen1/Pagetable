#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "mlpt.h"
#include "config.h"


size_t ptbr = 0; //global pagetable base register

size_t translate(size_t va) {
    if (ptbr == 0) {
        return ~0UL; //no memory allocated yet: all-1 bits
    }
    size_t *current_table = (size_t *)ptbr; //start at the root page

    int entries_per_table = (int)(pow(2, POBITS) / 8); //num of entries per table 
    int bits_for_vpn = (int)log2(entries_per_table); //num of bits needed to pick an entry in the table

    // number of bits to get rid of
    // to get to root level
    size_t level_shift = (bits_for_vpn * (LEVELS - 1)) + POBITS; 
    size_t vpn; //allocate space for virtual page number 

    //traverse all levels of page table
    for (int i = 0; i < LEVELS; i++){
        //get current level's VPN
        vpn = (va >> level_shift) & ((1 << bits_for_vpn) - 1);
        level_shift -= bits_for_vpn; //move down to the next level

        size_t pte = current_table[vpn]; //check if pts is valid
        if ((pte & 1) == 0){
            return ~0UL;
        }

        //get the result physical address if it's the last level
        if (i == LEVELS - 1){
            size_t ppn = pte >> POBITS; //the result physical page number
            size_t offset = va & ((1 << POBITS) - 1); //offset bits 
            return (ppn << POBITS) | offset; //final physical address
        }

        //move to next level page table
        size_t mask_offset = (1 << POBITS) - 1;
        current_table = (size_t *)(pte & ~mask_offset);
    }
    return ~0UL;
}

int allocate_page(size_t start_va) {
    // check alignment 
    if (start_va & ((1UL << POBITS) - 1)) {
        return -1; // not page aligned 
    }

    // set up constants
    int entries_per_table = (1 << POBITS) / 8; 
    int bits_for_vpn = (int)log2(entries_per_table); 
    size_t level_shift = (bits_for_vpn * (LEVELS - 1)) + POBITS; 

    // allocate root if not exist 
    if (ptbr == 0) {
        void *root; 
        posix_memalign(&root, (1UL << POBITS), (1UL << POBITS)); 
        memset(root, 0, (1UL << POBITS)); 
        ptbr = (size_t) root; 
    }

    size_t *current_table = (size_t *)ptbr; 
    size_t mask_offset = (1UL << POBITS) - 1; 

    // traverse levels 
    for (int i = 0; i < LEVELS; i++) {
        size_t vpn = (start_va >> level_shift) & ((1UL << bits_for_vpn) - 1); 
        level_shift -= bits_for_vpn; 
        size_t pte = current_table[vpn]; 

        // Last level = data page
        if (i == LEVELS - 1) {
            if (pte & 1) {
                // if valid mapping 
                return 0; 
            }

            void *data_page; 
            posix_memalign(&data_page, (1UL << POBITS), (1UL << POBITS)); 
            memset(data_page, 0, (1UL << POBITS)); 

            size_t ppn = (size_t)data_page >> POBITS; 
            current_table[vpn] = (ppn << POBITS) | 1; 
            return 1; 
        }

        //intermediate levels 
        if ((pte & 1) == 0) {
            void *new_table; 
            posix_memalign(&new_table, (1UL << POBITS), (1UL << POBITS)); 
            memset(new_table, 0, (1UL << POBITS)); 

            size_t new_ppn = (size_t)new_table >> POBITS;
            current_table[vpn] = (new_ppn << POBITS) | 1; 
            current_table = (size_t *)new_table; 
        } else {
            size_t next_addr = pte & ~mask_offset; 
            current_table = (size_t *)next_addr; 
        }
    }

    return 1; 
}