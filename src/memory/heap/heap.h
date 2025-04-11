#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>

// Each entry depecting the status of the block is of 8 bits (1 Byte)
#define HEAP_BLOCK_TABLE_ENRTY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000


//Because we want to store one byte of metadata per
// block in the heap, and unsigned char is exactly 1 byte.
typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table{
    HEAP_BLOCK_TABLE_ENTRY* entries; // This will point to the array(table) of block heap
    size_t total; // For 100 MB of Heap we need 25600 blocks
};

// This is the actual heap memory
struct heap{
    struct heap_table* table;
    void* saddr; // start address like 0x01000000
};

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table);

void* heap_malloc(struct heap* heap, size_t size);

void heap_free(struct heap* heap, void* ptr);

#endif