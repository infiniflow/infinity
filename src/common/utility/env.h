#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <sys/sysctl.h>
#else
#include <linux/falloc.h>
#include <linux/fiemap.h>
#include <linux/ioctl.h>
#include <linux/magic.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#endif  // defined(__APPLE__)

#ifdef __APPLE__
#define fdatasync fsync
#define fread_unlocked fread
#endif

#define RETRY_ON_EINTR(err, expr) do { \
  static_assert(std::is_signed<decltype(err)>::value, \
                #err " must be a signed integer"); \
  (err) = (expr); \
} while ((err) == -1 && errno == EINTR)

#if defined(__APPLE__)
// Simulates Linux's fallocate file preallocation API on OS X.
int fallocate(int fd, int mode, off_t offset, off_t len) {
    off_t size = offset + len;

    struct stat stat;
    int ret = fstat(fd, &stat);
    if (ret < 0) {
        return ret;
    }

    if (stat.st_blocks * 512 < size) {
        // The offset field seems to have no effect; the file is always allocated
        // with space from 0 to the size. This is probably because OS X does not
        // support sparse files.
        fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, size};
        if (fcntl(fd, F_PREALLOCATE, &store) < 0) {
            //LOG(INFO) << "Unable to allocate contiguous disk space, attempting non-contiguous allocation";
            store.fst_flags = F_ALLOCATEALL;
            ret = fcntl(fd, F_PREALLOCATE, &store);
            if (ret < 0) {
                return ret;
            }
        }
    }

    if (stat.st_size < size) {
        // fcntl does not change the file size, so set it if necessary.
        int ret;
        RETRY_ON_EINTR(ret, ftruncate(fd, size));
        return ret;
    }
    return 0;
}

#endif