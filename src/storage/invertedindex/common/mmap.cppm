module;

#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

import stl;
import infinity_exception;
export module mmap;

using namespace infinity;
namespace fs = std::filesystem;

namespace infinity {

export int MmapFile(const String &fp, u8 *&data_ptr, SizeT &data_len, int advice = (MADV_RANDOM | MADV_DONTDUMP)) {
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
    int rc = madvise(tmpd, len_f, advice);
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

export struct MmapReader {
    MmapReader(const String &filename, int advice = MADV_SEQUENTIAL) {
        int rc = MmapFile(filename, data_ptr_, data_len_, advice);
        idx_ = 0;
        if (rc < 0) {
            throw UnrecoverableException("MmapFile failed");
        }
    }

    ~MmapReader() {
        MunmapFile(data_ptr_, data_len_);
    }

    void Seek(SizeT diff) {
        idx_ += diff;
    }

    void ReadU64(u64 &val) {
        val = *(u64 *)(data_ptr_ + idx_);
        idx_ += sizeof(u64);
    }

    void ReadU32(u32 &val) {
        val = *(u32 *)(data_ptr_ + idx_);
        idx_ += sizeof(u32);
    }

    SizeT ReadBuf(char* buf, SizeT len) {
        if (idx_ + len < data_len_) {
            memcpy(buf, data_ptr_ + idx_, len);
            idx_ += len;
            return len;
        } else {
            SizeT left = data_len_ - idx_;
            memcpy(buf, data_ptr_ + idx_, left);
            idx_ = data_len_;
            return left;
        }
    }

    SizeT Tell() { return idx_; }

    SizeT DataLen() { return data_len_; }

    u8 *data_ptr_ = nullptr;

    SizeT data_len_{0};

    SizeT idx_{0};
};

} // namespace infinity
