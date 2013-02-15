#ifndef LIBCHIKEN_SYS_STAT_H
#define LIBCHIKEN_SYS_STAT_H

#include <sys/types.h>

// This struct appears here as it is defined in IEEE 1003.1, 2003 ed. (POSIX)
// The standard can be obtained at <http://www.opengroup.org/unix/online.html>
struct stat
{
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
};

int fstat(int, struct stat*);
int stat(const char *restrict, struct stat *restrict);

#endif

