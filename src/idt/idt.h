#ifndef IDT_H
#define IDT_H
#include <stdint.h>

// Define the structure of a single IDT entry
struct idt_desc {
    uint16_t offset_low;    // Lower 16 bits of the interrupt handler address
    uint16_t selector;      // Code segment selector for the interrupt handler
    uint8_t zero;           // Reserved byte, usually set to zero
    uint8_t type_attr;      // Gate type and attributes (e.g., interrupt type, privilege level)
    uint16_t offset_high;   // Upper 16 bits of the interrupt handler address
} __attribute__((packed));

// Define the structure for the IDT pointer
struct idtr_desc {
    uint16_t limit; // Limit of the IDT (size - 1)
    uint32_t base; // Base address of the start of the IDT
} __attribute__((packed));

void idt_init();
void enable_interrupts();
void diable_interrupts();


#endif
