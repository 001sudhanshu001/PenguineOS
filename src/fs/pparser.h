#ifndef PATHPARSER_H
#define PATHPARSER_H

// This will kind of LinkedList, each node is after slash
struct path_root {
    int drive_no;
    struct path_part* first; // Pointer to first part of the path, eg. in 0:/dir/file, it will point to dir
};

struct path_part{
    const char* part;
    struct path_part* next;
};


#endif