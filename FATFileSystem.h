#ifndef MBED_FATFILESYSTEM_H
#define MBED_FATFILESYSTEM_H

#include "FileSystemLike.h"
#include "FileHandle.h"
#include "ff.h"
#include <stdint.h>

using namespace mbed;

class FATFileSystem : public FileSystemLike {
public:

    FATFileSystem(const char* n);
    virtual ~FATFileSystem();

    static FATFileSystem * _ffs[_VOLUMES];   // FATFileSystem objects, as parallel to FatFs drives array
    FATFS _fs;                               // Work area (file system object) for logical drive
    char _fsid[2];

    virtual FileHandle *open(const char* name, int flags);
    virtual int remove(const char *filename);
    virtual int rename(const char *oldname, const char *newname);
    virtual DirHandle *opendir(const char *name);
    virtual int mkdir(const char *name, mode_t mode);
    virtual int mount();
    
    virtual int unmount();

    virtual int disk_initialize() { return 0; }
    virtual int disk_status() { return 0; }
    virtual int disk_read(uint8_t *buffer, uint32_t sector, uint32_t count) = 0;
    virtual int disk_write(const uint8_t *buffer, uint32_t sector, uint32_t count) = 0;
    virtual int disk_sync() { return 0; }
    virtual uint32_t disk_sectors() = 0;

};

#endif
