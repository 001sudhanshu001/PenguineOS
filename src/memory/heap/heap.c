#include "heap.h"
#include "kernel.h"
#include <stdbool.h>
#include "memory/memory.h"
#include "status.h"
#include "config.h"


// This check is done only once during heap initialization 
static int heap_validate_table(void* ptr, void* end, struct heap_table* table) {
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size/PENGUINEOS_HEAP_BLOCK_SIZE_BYTES;

    if(table-> total != total_blocks) {
        res = -EINVARG;
        goto out;
    }
    out:
        return res;
}

static bool heap_validate_alignment(void* ptr) {
    return ((unsigned int)ptr % PENGUINEOS_HEAP_BLOCK_SIZE_BYTES) == 0;
}

// This heap_create function will be called just once to initialize the heap at bootup
// 0 -> everything worked fine
// any -ve no. -> something went wrong
int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table) {
    int res = 0;

    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)) {
        res = -EINVARG;
        goto out;
    }

    // Cleaning the heap
    memset(heap, 0, sizeof(struct heap));
    heap->saddr = ptr;
    heap->table = table;

    res = heap_validate_table(ptr, end, table);

    if(res < 0) goto out;

    // Cleaning the heap_entry_table
    // sizeof(HEAP_BLOCK_TABLE_ENTRY) == 1 as it is unsigned char
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;  // 25600
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size); // HEAP_BLOCK_TABLE_ENTRY_FREE = 0x00, means setting each byte as 0

    out:
        return res;
}


static uint32_t heap_align_value_to_uppper(uint32_t bytes_to_be_allocated) {
    if((bytes_to_be_allocated % PENGUINEOS_HEAP_BLOCK_SIZE_BYTES) == 0) {
        return bytes_to_be_allocated;
    }

    bytes_to_be_allocated = (bytes_to_be_allocated - (bytes_to_be_allocated % PENGUINEOS_HEAP_BLOCK_SIZE_BYTES));
    bytes_to_be_allocated += PENGUINEOS_HEAP_BLOCK_SIZE_BYTES;

    return bytes_to_be_allocated;
}

// This will return the first 4 bits(from left) 
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0x0f; // This will give first 4 Bits
}

int heap_get_start_block(struct heap* heap, int blocks_to_be_allocated) {
    struct  heap_table* table = heap->table;
    int current_block = 0;
    int start_block = -1;

    for(size_t i = 0; i < table->total; i++) {
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            current_block = 0;
            start_block = -1;
            continue;
        }

        if(start_block == -1) { // Means this is the first free block
            start_block = i;
        }
        current_block++;
        if(current_block == blocks_to_be_allocated) {
            break;
        }
    }

    if(start_block == -1) return -ENOMEM;

    return start_block;
    
}

void* heap_block_to_address(struct heap* heap, int block) {
    return heap->saddr + (block + PENGUINEOS_HEAP_BLOCK_SIZE_BYTES);
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int allocated_blocks) {
    int end_block = (start_block + allocated_blocks) - 1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENRTY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if(allocated_blocks > 1) {
        entry = entry | HEAP_BLOCK_HAS_NEXT;
    }

    for(int i = start_block; i <= end_block; i++) {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENRTY_TAKEN;
        if(i != end_block - 1) {
            entry = entry | HEAP_BLOCK_HAS_NEXT;
        }
    }

}
// Here we will find the free Blocks from the heap and return the pointer to the first block
void* heap_malloc_blocks(struct heap* heap, uint32_t blocks_to_be_allocated) {
    void* address = 0;
    
    int start_block = heap_get_start_block(heap, blocks_to_be_allocated); // see if we can find the continuos blocks
    if(start_block < 0) {
        goto out;
    }

    address = heap_block_to_address(heap, start_block); // finding the actual memory address of the start block


    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, blocks_to_be_allocated);

    out:
    return address;
}

void* heap_malloc(struct heap* heap, size_t bytes_to_be_allocated) {
    size_t aligned_size = heap_align_value_to_uppper(bytes_to_be_allocated);

    // finding the no. of block to be allocated
    uint32_t blocks_to_be_allocated = aligned_size/PENGUINEOS_HEAP_BLOCK_SIZE_BYTES;

    return heap_malloc_blocks(heap, blocks_to_be_allocated);
}


void heap_mark_blocks_free(struct heap* heap, int starting_block){
    struct heap_table* table = heap->table;
 
    for (int i = starting_block; i < (int)table->total; i++) {

        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];

        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break;
        }
    }

}

int heap_address_to_block(struct heap* heap, void* address) {
    return ((int)(address - heap->saddr)) / PENGUINEOS_HEAP_BLOCK_SIZE_BYTES;
}

void heap_free(struct heap* heap, void* ptr) {
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}