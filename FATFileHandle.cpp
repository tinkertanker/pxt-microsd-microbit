#include "ff.h"
#include "ffconf.h"
#include "mbed_debug.h"

#include "FATFileHandle.h"

FATFileHandle::FATFileHandle(FIL fh) {
    _fh = fh;
}

int FATFileHandle::close() {
    int retval = f_close(&_fh);
    delete this;
    return retval;
}

ssize_t FATFileHandle::write(const void* buffer, size_t length) {
    UINT n;
    FRESULT res = f_write(&_fh, buffer, length, &n);
    if (res) {
        debug_if(FFS_DBG, "f_write() failed: %d", res);
        return -1;
    }
    return n;
}

ssize_t FATFileHandle::read(void* buffer, size_t length) {
    debug_if(FFS_DBG, "read(%d)\n", length);
    UINT n;
    FRESULT res = f_read(&_fh, buffer, length, &n);
    if (res) {
        debug_if(FFS_DBG, "f_read() failed: %d\n", res);
        return -1;
    }
    return n;
}

int FATFileHandle::isatty() {
    return 0;
}

off_t FATFileHandle::lseek(off_t position, int whence) {
    if (whence == SEEK_END) {
        position += _fh.fsize;
    } else if(whence==SEEK_CUR) {
        position += _fh.fptr;
    }
    FRESULT res = f_lseek(&_fh, position);
    if (res) {
        debug_if(FFS_DBG, "lseek failed: %d\n", res);
        return -1;
    } else {
        debug_if(FFS_DBG, "lseek OK, returning %i\n", _fh.fptr);
        return _fh.fptr;
    }
}

int FATFileHandle::fsync() {
    FRESULT res = f_sync(&_fh);
    if (res) {
        debug_if(FFS_DBG, "f_sync() failed: %d\n", res);
        return -1;
    }
    return 0;
}

off_t FATFileHandle::flen() {
    return _fh.fsize;
}
