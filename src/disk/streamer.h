#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H


#include "disk.h"

struct disk_stream {
    int position; // This is byte position not sector
    struct disk* disk;
};

struct disk_stream* diskstreamer_new(int disk_id);

int diskstreamer_seek(struct disk_stream* stream, int position);

int diskstreamer_read(struct disk_stream* stream, void* out, int total_bytes_to_be_read);

void diskstreamer_close(struct disk_stream* stream);

#endif