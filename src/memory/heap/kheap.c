#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"

struct heap kernel_heap;

struct heap_table kernel_heap_table; // The table which will hold the state of each memory block

void kheap_init() {
    int total_table_entries = PENGUINEOS_HEAP_SIZE_BYTES/PENGUINEOS_HEAP_BLOCK_SIZE_BYTES; // 25600
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) (PENGUINEOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries; //25600

    void* end = (void*) (PENGUINEOS_HEAP_ADDRESS + PENGUINEOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(PENGUINEOS_HEAP_ADDRESS), end, &kernel_heap_table);

    if(res < 0) {
        print("FAILED TO CREATE HEAP\n");
    }
}

void* kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size) {
    void* ptr = kmalloc(size);
    if(!ptr) {
        return 0;
    }
    memset(ptr, 0x00, size);
    return ptr;;
}

void kfree(void* ptr) {
    heap_free(&kernel_heap, ptr);
}