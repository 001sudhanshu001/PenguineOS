#include "memory.h"

// Making all values 0, starting from the first byte of ptr which is idt_descriptors
void* memset(void* ptr, int val, size_t size) {
    char* c_ptr = (char*) ptr; // Targetting each Byte

    for (int i = 0; i < size; i++) {
        c_ptr[i] = (char) val;
    }

    return ptr;
}
