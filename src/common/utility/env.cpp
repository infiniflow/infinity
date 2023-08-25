#include "env.h"

#include <type_traits>

#if defined(__APPLE__)

#define RETRY_ON_EINTR(err, expr) do { \
  static_assert(std::is_signed<decltype(err)>::value, \
                #err " must be a signed integer"); \
  (err) = (expr); \
} while ((err) == -1 && errno == EINTR)

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