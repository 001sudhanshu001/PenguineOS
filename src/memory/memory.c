#include "memory.h"

// Making all values 0, starting from the first byte of ptr which is idt_descriptors
void* memset(void* ptr, int val, size_t size) {
    char* c_ptr = (char*) ptr; // Targetting each Byte

    for (int i = 0; i < size; i++) {
        c_ptr[i] = (char) val;
    }

    return ptr;
}

// ptr1: Pointer to the first block of memory
// ptr2: Pointer to the second block of memory
// num: Number of bytes to compare
int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const char* p1 = ptr1;
    const char* p2 = ptr2;

    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }

    return 0;
}
