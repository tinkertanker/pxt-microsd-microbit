#include "mbed.h"

#include "ffconf.h"
#include "mbed_debug.h"

#include "FATFileSystem.h"
#include "FATFileHandle.h"
#include "FATDirHandle.h"

DWORD get_fattime(void) {
    time_t rawtime;
    time(&rawtime);
    struct tm *ptm = localtime(&rawtime);
    return (DWORD)(ptm->tm_year - 80) << 25
         | (DWORD)(ptm->tm_mon + 1  ) << 21
         | (DWORD)(ptm->tm_mday     ) << 16
         | (DWORD)(ptm->tm_hour     ) << 11
         | (DWORD)(ptm->tm_min      ) << 5
         | (DWORD)(ptm->tm_sec/2    );
}

FATFileSystem *FATFileSystem::_ffs[_VOLUMES] = {0};

FATFileSystem::FATFileSystem(const char* n) : FileSystemLike(n) {
    debug_if(FFS_DBG, "FATFileSystem(%s)\n", n);
    for(int i=0; i<_VOLUMES; i++) {
        if(_ffs[i] == 0) {
            _ffs[i] = this;
            _fsid[0] = '0' + i;
            _fsid[1] = '\0';
            debug_if(FFS_DBG, "Mounting [%s] on ffs drive [%s]\n", getName(), _fsid);
            f_mount(&_fs, _fsid, 0);
            return;
        }
    }
    error("Couldn't create %s in FATFileSystem::FATFileSystem\n", n);
}

FATFileSystem::~FATFileSystem() {
    for (int i=0; i<_VOLUMES; i++) {
        if (_ffs[i] == this) {
            _ffs[i] = 0;
            f_mount(NULL, _fsid, 0);
        }
    }
}

FileHandle *FATFileSystem::open(const char* name, int flags) {
    debug_if(FFS_DBG, "open(%s) on filesystem [%s], drv [%s]\n", name, getName(), _fsid);
    char n[64];
    sprintf(n, "%s:/%s", _fsid, name);

    /* POSIX flags -> FatFS open mode */
    BYTE openmode;
    if (flags & O_RDWR) {
        openmode = FA_READ|FA_WRITE;
    } else if(flags & O_WRONLY) {
        openmode = FA_WRITE;
    } else {
        openmode = FA_READ;
    }
    if(flags & O_CREAT) {
        if(flags & O_TRUNC) {
            openmode |= FA_CREATE_ALWAYS;
        } else {
            openmode |= FA_OPEN_ALWAYS;
        }
    }

    FIL fh;
    FRESULT res = f_open(&fh, n, openmode);
    if (res) {
        debug_if(FFS_DBG, "f_open('w') failed: %d\n", res);
        return NULL;
    }
    if (flags & O_APPEND) {
        f_lseek(&fh, fh.fsize);
    }
    return new FATFileHandle(fh);
}

int FATFileSystem::remove(const char *filename) {
    FRESULT res = f_unlink(filename);
    if (res) {
        debug_if(FFS_DBG, "f_unlink() failed: %d\n", res);
        return -1;
    }
    return 0;
}

int FATFileSystem::rename(const char *oldname, const char *newname) {
    FRESULT res = f_rename(oldname, newname);
    if (res) {
        debug_if(FFS_DBG, "f_rename() failed: %d\n", res);
        return -1;
    }
    return 0;
}

DirHandle *FATFileSystem::opendir(const char *name) {
    FATFS_DIR dir;
    FRESULT res = f_opendir(&dir, name);
    if (res != 0) {
        return NULL;
    }
    return new FATDirHandle(dir);
}

int FATFileSystem::mkdir(const char *name, mode_t mode) {
    FRESULT res = f_mkdir(name);
    return res == 0 ? 0 : -1;
}

int FATFileSystem::mount() {
    FRESULT res = f_mount(&_fs, _fsid, 1);
    return res == 0 ? 0 : -1;
}

int FATFileSystem::unmount() {
    if (disk_sync())
        return -1;
    FRESULT res = f_mount(NULL, _fsid, 0);
    return res == 0 ? 0 : -1;
}
