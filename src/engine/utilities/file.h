#ifndef ENGINE_UTILITIES_FILE_H
#define ENGINE_UTILITIES_FILE_H

#include "integers.h"
#include "inline.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#else
#error unsupported platform
#endif

INLINE size_t readFile(const char* path, u8** data) {
    FILE* file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    *data = malloc(size);
    fread(*data, size, 1, file);
    fclose(file);
    return size;
}

typedef struct MappedFile {
#ifdef __linux__
    void* memory;
    size_t size;
    int fd;
#endif
} MappedFile;

INLINE MappedFile memoryMapFile(const char* path) {
#ifdef __linux__
    MappedFile file = {.fd = open(path, O_RDONLY, 0644)};
    struct stat sb;
    fstat(file.fd, &sb);
    file.size = sb.st_size;
    file.memory = mmap(NULL, file.size, PROT_READ, MAP_SHARED, file.fd, 0);
    return file;
#endif
}

INLINE void memoryUnmapFile(MappedFile file) {
#ifdef __linux__
    msync(file.memory, file.size, MS_SYNC);
    munmap(file.memory, file.size);
    close(file.fd);
#endif
}

#endif
