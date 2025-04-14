#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"

struct disk disk; // Represent primary hardisk

int disk_read_sector(int lba, int total_sector_to_read, void* buffer) {
    // Step 1: Set the Drive/Head register
    // 0xE0 selects Master drive and enables LBA
    outb(0x1F6, (lba >> 24) | 0xE0);

    // Step 2: Send the number of sectors to read
    outb(0x1F2, total_sector_to_read);

    // Step 3: Send the 28-bit LBA address, split into 3 registers
    outb(0x1F3, (unsigned char)(lba & 0xFF));        // LBA bits 0-7
    outb(0x1F4, (unsigned char)(lba >> 8));          // LBA bits 8-15
    outb(0x1F5, (unsigned char)(lba >> 16));         // LBA bits 16-23

    // Step 4: Send the Read command to the Command register
    outb(0x1F7, 0x20); // 0x20 = READ SECTORS command

    // Step 5: Prepare to copy disk data into buffer
    unsigned short* ptr = (unsigned short*) buffer;

    // Step 6: Read the data for each sector
    for (int b = 0; b < total_sector_to_read; b++) {
        // Wait for the disk to be ready (polling)
        // Bit 3 (0x08) of the status register must be set (DRQ - Data Request)
        char ch = insb(0x1F7);
        while (!(ch & 0x08)) {
            ch = insb(0x1F7); // Re-read status until ready
        }

        // Step 7: Read 512 bytes = 256 words (16-bit reads)
        for (int i = 0; i < 256; i++) {
            *ptr = insw(0x1F0); // Read a word from data port
            ptr++;              // Move to next word in buffer
        }
    }

    return 0; // success
}

void disk_search_and_init() {
    memset(&disk, 0, sizeof(disk));
    disk.type = PENGUINEOS_DISK_TYPE_REAL;
    disk.sector_size = PENGUINEOS_SECTOR_SIZE;
}

struct disk* disk_get(int index) {
    if(index != 0) return 0;

    return &disk;  // As we have only one disk for now
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total_sector_to_read, void* buffer) {
    if(idisk != &disk) {
        return -EIO;
    }

    return disk_read_sector(lba, total_sector_to_read, buffer);
}