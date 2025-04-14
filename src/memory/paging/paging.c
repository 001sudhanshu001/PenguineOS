#include "paging.h"
#include "status.h"
#include "memory/heap/kheap.h"

static uint32_t* current_directory = 0;


void paging_load_directory(uint32_t* directory);

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for(int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* page_table_entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for(int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++) {
            page_table_entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)page_table_entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;

}

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;

}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}

bool paging_is_aligned(void* addr) {
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

// Takes Virtual Address and calculate the directory index and page table index
int paging_get_indexes(void* virtual_addres, uint32_t* directory_index_out, uint32_t* table_index_out) {
    int res = 0;
    if(!paging_is_aligned(virtual_addres)) { // Checking if the address is valid for 4096
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_addres / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_addres % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

    out:
        return res;
}

int paging_set(uint32_t* directory, void* virtual_address, uint32_t val) { // val contains Physical address along with flags
    if(!paging_is_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if(res < 0) {
        return res;
    }
    
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000); // Extracting top 20 Bits which has Page Table 4-kb aligned Address

    table[table_index] = val;

    return 0;
}