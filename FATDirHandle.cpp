#include <string.h>
#include "ff.h"
#include "FATDirHandle.h"

using namespace mbed;

FATDirHandle::FATDirHandle(const FATFS_DIR &the_dir) {
    dir = the_dir;
}

int FATDirHandle::closedir() {
    int retval = f_closedir(&dir);
    delete this;
    return retval;
}

struct dirent *FATDirHandle::readdir() {
    FILINFO finfo;

#if _USE_LFN
    finfo.lfname = cur_entry.d_name;
    finfo.lfsize = sizeof(cur_entry.d_name);
#endif // _USE_LFN

    FRESULT res = f_readdir(&dir, &finfo);

#if _USE_LFN
    if(res != 0 || finfo.fname[0]==0) {
        return NULL;
    } else {
        if(cur_entry.d_name[0]==0) {
            memcpy(cur_entry.d_name, finfo.fname, sizeof(finfo.fname));
        }
        return &cur_entry;
    }
#else
    if(res != 0 || finfo.fname[0]==0) {
        return NULL;
    } else {
        memcpy(cur_entry.d_name, finfo.fname, sizeof(finfo.fname));
        return &cur_entry;
    }
#endif
}

void FATDirHandle::rewinddir() {
    dir.index = 0;
}

off_t FATDirHandle::telldir() {
    return dir.index;
}

void FATDirHandle::seekdir(off_t location) {
    dir.index = location;
}

