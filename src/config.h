#ifndef CONFIG_H
#define CONFIG_H
 

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define PENGUINEOS_TOTAL_INTERRUPTS 512

// 100 MB heap size
#define PENGUINEOS_HEAP_SIZE_BYTES 104857600 
#define PENGUINEOS_HEAP_BLOCK_SIZE_BYTES 4096 // 4 KB
#define PENGUINEOS_HEAP_ADDRESS 0x01000000 // Starting address of heap, at 16 MB
#define PENGUINEOS_HEAP_TABLE_ADDRESS 0x00007E00 // The address where the table which stores the state of each memeory block
// This table need 25600 Bytes
#endif

