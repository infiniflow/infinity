module;

#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
import stl;

export module mmap;

using namespace infinity;
namespace fs = std::filesystem;

namespace infinity {

export int MmapFile(const String &fp, u8 *&data_ptr, SizeT &data_len) {
    data_ptr = nullptr;
    data_len = 0;
    long len_f = fs::file_size(fp);
    if (len_f == 0)
        return -1;
    int f = open(fp.c_str(), O_RDONLY);
    void *tmpd = mmap(NULL, len_f, PROT_READ, MAP_SHARED, f, 0);
    if (tmpd == MAP_FAILED)
        return -1;
    close(f);
    int rc = madvise(tmpd, len_f, MADV_RANDOM | MADV_DONTDUMP);
    if (rc < 0)
        return -1;
    data_ptr = (u8 *)tmpd;
    data_len = len_f;
    return 0;
}

export int MunmapFile(u8 *&data_ptr, SizeT &data_len) {
    if (data_ptr != nullptr) {
        int rc = munmap(data_ptr, data_len);
        if (rc < 0)
            return -1;
        data_ptr = nullptr;
        data_len = 0;
    }
    return 0;
}

} // namespace infinity
