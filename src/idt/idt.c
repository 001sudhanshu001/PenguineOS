#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "kernel.h"
#include "io/io.h"

struct idt_desc idt_descriptors[PENGUINEOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

void int21_handler() {
    print("Keyboard Pressed ! \n");
    outb(0x20, 0x20);
}

void no_interrupt_handler() {
    outb(0x20,0x20);
}

void idt_zero() {
    print("Divide by Zero error\n");
}

void idt_set(int interrupt_no, void* address) {
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_low = (int32_t) address & 0x000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_high = (uint32_t) address >> 16;
}

void idt_init() {
    // Clearing the memory, so that no garbage value is present in idt_descriptors
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;


    for(int i = 0; i < PENGUINEOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, no_interrupt);
    }
    // CPU is hardwired/preprogrammed to  Automatically invoke Interrupt Vector Number 0 (IDT[0])
    // whenever there is a divide-by-zero operation.
    idt_set(0, idt_zero); // For Interrupt no. 0 Invoke idt_zero() function
    idt_set(0x21, int21h); // 0x21 int for keypress, send by PIC

    // load the IDT
    idt_load(&idtr_descriptor);
}