#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"

struct disk_stream* diskstreamer_new(int disk_id) {
    struct disk* disk = disk_get(disk_id);
    if(!disk) return 0;

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->position = 0; // At Byte 0
    streamer->disk = disk;

    return streamer;
}


// This will reposition in the disk stream to the position provided
int diskstreamer_seek(struct disk_stream* stream, int position) {
    stream->position = position;

    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total_bytes_to_be_read) {
    int sector = stream->position / PENGUINEOS_SECTOR_SIZE; // Finding the sector number
    int offset = stream->position % PENGUINEOS_SECTOR_SIZE;

    char buffer[PENGUINEOS_SECTOR_SIZE];

    // At once we have to read complete one sector (512 Bytes)
    int res = disk_read_block(stream->disk, sector, 1, buffer);

    if(res < 0) goto out;

    int temp_total_bytes_to_be_read = total_bytes_to_be_read > PENGUINEOS_SECTOR_SIZE ? PENGUINEOS_SECTOR_SIZE : total_bytes_to_be_read;

    // Giving only the asked amount
    for(int i = 0; i < temp_total_bytes_to_be_read; i++) {
        *(char*)out++ = buffer[offset + i];
    }

    // Adjust the String(What if the total bytes to be read is more than that of SECTOR_SIZE i.e. 512 Bytes)
    stream->position += total_bytes_to_be_read;
    if(total_bytes_to_be_read > PENGUINEOS_SECTOR_SIZE) {
        res = diskstreamer_read(stream, out, total_bytes_to_be_read - PENGUINEOS_HEAP_SIZE_BYTES); // Recursion
    }
 
    out: 
        return res;
}

void diskstreamer_close(struct disk_stream* stream) {
    kfree(stream);
} 