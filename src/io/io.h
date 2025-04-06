#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port); // read one byte from the given port
unsigned short insw(unsigned short port); // Read word from the given port

void outb(unsigned short port, unsigned char val); // Outpur 1 bytes to the port
void outw(unsigned short port, unsigned short val); // Output 2 bytes to the port

#endif