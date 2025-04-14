#ifndef DISK_H
#define DISK_H

int disk_read_sector(int lba, int total_blocks_to_read, void* buffer);

#endif