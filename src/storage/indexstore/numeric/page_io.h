// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <filesystem>
#include <iostream>
#include <leveldb/status.h>

#include "main/logger.h"
#include "common/utility/env.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/spinlock.h"

namespace infinity {

using Status = leveldb::Status;

#define CHECK_ERROR(call)                                                    \
  do {                                                                       \
    if ((call) < 0) {                                                        \
      const char* error = strerror(errno);                                   \
      std::cerr << __FILE__ << ":" << __LINE__ << " " << error << std::endl; \
      exit(1);                                                               \
    }                                                                        \
  } while (0)

class Page;

class File {
public:
    File(const std::filesystem::path& name);

    ~File() {
        close(fd_);
    }

    void Close() {
        close(fd_);
    }

    void Create();

    void Open(bool read_only = false);

    void Read(uint64_t addr, void *buffer, size_t len);

    void Write(size_t addr, void* buffer, size_t len);

    void Flush() {
        CHECK_ERROR(fsync(fd_));
    }

    bool IsOpen() const {
        return fd_ != -1;
    }

    void Seek(uint64_t offset, int whence) const;

    uint64_t Tell() const;

    uint64_t FileSize() const;

    void Truncate(uint64_t newsize);

    uint64_t Alloc(size_t requested_length);

    void AllocPage(Page *page);

    void FreePage(Page *page);

    void ReadPage(Page *page, uint64_t address);

    void ReclaimSpace();
private:
    std::string filename_;
    int fd_;
    size_t file_size_;
    size_t excess_at_end_;
    SpinLock mutex_;
};


}
