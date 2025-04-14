#ifndef DISK_H
#define DISK_H

typedef unsigned int PENGUINEOS_DISK_TYPE;

// Represents a real physical hard disk
#define PENGUINEOS_DISK_TYPE_REAL 0

struct disk {
    PENGUINEOS_DISK_TYPE type;
    int sector_size;
};

void disk_search_and_init();

struct disk* disk_get(int index);

int disk_read_block(struct disk* idisk, unsigned int lba, int total_sector_to_read, void* buffer);

#endif