#include "pparser.h"
#include "kernel.h"
#include "string/string.h"
#include "memory/memory.h"
#include "memory/memory.h"
#include "status.h"
#include "memory/heap/kheap.h"

// Example Path - 0:/dir/file
static int pathparser_path_valid_format(const char* filename) {
    int len = strnlen(filename, PENGUINEOS_MAX_PATH);   
    return (len >= 3) && isdigit(filename[0]) && memcmp((void*)&filename, ":/", 2) == 0;
}

static int pathparser_get_drive_by_path(const char** path) {
    if(!pathparser_path_valid_format(*path)) {
        return -EBADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    // Add 3 Bytes to skip drive number i.e. 0:1
    *path += 3;

    return drive_no;

}

static struct path_root* pathparser_create_root(int drive_number) {
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = NULL;

    return path_r;
}


// The input to this function will be :: bin/file.txt and will return bin
static const char* pathparser_get_path_part(const char** path) { // ** beacuse we need to update the pointer itself
    char* result_path_part = kzalloc(PENGUINEOS_MAX_PATH);
    int i = 0;
    while(**path != '/' && **path != 0X00) {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if(**path == '/') { // skip the forward slash
        *path += 1;
    }

    if(i == 0) { // means we did not parse anything
        kfree(result_path_part);
        result_path_part = 0; // delete pointer
    }

    return result_path_part;
}


struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path) {
    const char* path_part_str = pathparser_get_path_part(path);
    if(!path_part_str) {
        return 0;
    }

    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part =  path_part_str;
    part->next = NULL;

    if(last_part) {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct path_root* root) {
    struct  path_part* part = root->first;
    while(part) {
        struct path_part* next_part = part->next;
        kfree((void*) part->part);
        kfree(part);
        part = next_part;
    }

    kfree(part);
}

// This method will be exposed for kernel
struct path_root* pathparser_parse(const char* path, const char* current_directory_path) {
    int res = 0;
    const char* temp_path = path;
    struct path_root* path_root = 0;

    if(strlen(path) > PENGUINEOS_MAX_PATH) goto out;

    res = pathparser_get_drive_by_path(&temp_path);
    if(res < 0) goto out;

    path_root = pathparser_create_root(res);
    if(!path_root) goto out;

    struct path_part* first_part = pathparser_parse_path_part(NULL, &temp_path);

    if(!first_part) goto out;

    path_root->first = first_part;

    struct path_part* part = pathparser_parse_path_part(first_part, &temp_path);

    while(part) {
        part = pathparser_parse_path_part(part, &temp_path);
    }

    out:
        return path_root;
}