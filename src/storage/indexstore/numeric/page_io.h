#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <filesystem>

#include "page.h"
#include "common/utility/env.h"

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

class File {
    // The number of pages by which to grow a file when needed.
    const size_t kGrowthPages = 256;

public:
    File(const std::filesystem::path& name, size_t initial_num_pages);

    ~File() {
        close(fd_);
    }

    Status ReadPage(size_t offset, void* data) const {
        if (offset >= next_page_allocation_offset_) {
            return Status::InvalidArgument("Tried to read from unallocated page.");
        }
        CHECK_ERROR(pread(fd_, data, Page::kSize, offset));
        return Status::OK();
    }

    Status WritePage(size_t offset, const void* data) const {
        if (offset >= next_page_allocation_offset_) {
            return Status::InvalidArgument("Tried to write to unallocated page.");
        }
        CHECK_ERROR(pwrite(fd_, data, Page::kSize, offset));
        return Status::OK();
    }
    void Sync() const {
        CHECK_ERROR(fsync(fd_));
    }

    size_t AllocatePage() {
        size_t allocated_offset = next_page_allocation_offset_;
        next_page_allocation_offset_ += Page::kSize;

        ExpandToIfNeeded(allocated_offset);
        return allocated_offset;
    }

private:
    void ExpandToIfNeeded(size_t offset);

    int fd_;
    size_t file_size_;
    const size_t growth_bytes_;
    size_t next_page_allocation_offset_;
};

class PageIO {
public:
    PageIO(std::filesystem::path db_path);

    Status ReadPage(const PhysicalPageId physical_page_id, void* data);

    Status WritePage(const PhysicalPageId physical_page_id, void* data);

    PhysicalPageId AllocatePage();

    size_t GetNumPages() const {
        return total_pages_;
    }

private:
    std::unique_ptr<File> file_;

    size_t total_pages_;

    std::mutex page_allocation_mutex_;
};

}
