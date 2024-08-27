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

export int MmapFile(const String &fp,
                    u8 *&data_ptr,
                    SizeT &data_len,
                    int advice = (MADV_RANDOM
#if defined(linux) || defined(__linux) || defined(__linux__)
                                  | MADV_DONTDUMP
#endif
                                  )) {
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

export int MunmapFile(u8 *&data_ptr, SizeT &data_len, SizeT offset_diff = 0) {
    if (data_ptr != nullptr) {
        int rc = munmap(data_ptr - offset_diff, data_len + offset_diff);
        if (rc < 0)
            return -1;
        data_ptr = nullptr;
        data_len = 0;
    }
    return 0;
}

export struct MmapReader {
    MmapReader(const String &filename, SizeT offset = 0, SizeT len = SizeT(-1), int advice = MADV_SEQUENTIAL) {
        int rc = MmapPartFile(filename, data_ptr_, len, advice, offset);
        idx_ = 0;
        data_len_ = len;
        if (rc < 0) {
            UnrecoverableError("MmapFile failed");
        }
    }

    ~MmapReader() {
        MunmapFile(data_ptr_, data_len_, offset_diff_);
    }

    void Seek(SizeT pos, bool set = false) {
        if (set) {
            idx_ = pos;
        } else {
            idx_ += pos;
        }
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
        if (idx_ + len <= data_len_) {
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

    char* ReadBufNonCopy(SizeT len) {
        char* buf = (char*)(data_ptr_ + idx_);
        idx_ = std::min(idx_ + len, data_len_);
        return buf;
    }

    int MmapPartFile(const String &fp,
                     u8 *&data_ptr,
                     SizeT &data_len,
                     int advice = (MADV_RANDOM
#if defined(linux) || defined(__linux) || defined(__linux__)
                                   | MADV_DONTDUMP
#endif
                                   ),
                     SizeT offset = 0) {
        data_ptr = nullptr;
        long len_f = fs::file_size(fp);
        if (len_f == 0) {
            return -1;
        }
        if (data_len == SizeT(-1)) {
            data_len = len_f;
        } else if (data_len > (SizeT)len_f) {
            return -1;
        }

        SizeT page_size = getpagesize();

        SizeT aligned_offset = offset & ~(page_size - 1);
        offset_diff_ = offset - aligned_offset;

        SizeT mapped_length = data_len + offset_diff_;

        int f = open(fp.c_str(), O_RDONLY);
        void *tmpd = mmap(NULL, mapped_length, PROT_READ, MAP_SHARED, f, aligned_offset);
        if (tmpd == MAP_FAILED)
            return -1;
        close(f);
        int rc = madvise(tmpd, data_len, advice);
        if (rc < 0)
            return -1;
        data_ptr = (u8 *)tmpd + offset_diff_;
        return 0;
    }

    SizeT Tell() { return idx_; }

    SizeT DataLen() { return data_len_; }

    u8 *data_ptr_ = nullptr;

    SizeT data_len_{0};

    SizeT idx_{0};

    SizeT offset_diff_{0};
};

} // namespace infinity
