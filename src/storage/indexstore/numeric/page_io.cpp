#include "page_io.h"
#include "page.h"

#include <memory>

namespace infinity {

File::File(const std::filesystem::path& name)
    : filename_(name),
      fd_(-1),
      file_size_(0),
      excess_at_end_(0) {
}

void
File::Create() {
    CHECK_ERROR(
        fd_ = open(filename_.c_str(),
                   O_CREAT | O_RDWR | O_TRUNC |
#if defined(__linux__)
                   O_DIRECT
#else
                   0
#endif
                   ,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

}

void
File::Open(bool read_only) {
    int osflags = 0;
    if (read_only)
        osflags |= O_RDONLY;
    else
        osflags |= O_RDWR;
    CHECK_ERROR(fd_ = open(filename_.c_str(), osflags));
    file_size_ = FileSize();
}

void
File::Seek(uint64_t offset, int whence) const {
    CHECK_ERROR (lseek(fd_, offset, whence) < 0);
}

uint64_t
File::Tell() const {
    uint64_t offset = lseek(fd_, 0, SEEK_CUR);
    CHECK_ERROR (offset < 0);
    return offset;
}

uint64_t
File::FileSize() const {
    Seek(0, SEEK_END);
    uint64_t size = Tell();
    return size;
}

void
File::Read(uint64_t addr, void *buffer, size_t len) {
    ScopedSpinLock lock(mutex_);
    int r;
    size_t total = 0;

    while (total < len) {
        r = pread(fd_, (uint8_t *)buffer + total, len - total,
                  addr + total);
        if (r < 0) {
            throw StorageException(fmt::format("file {}. Read error : {}", filename_, strerror(errno)));
        }
        if (r == 0)
            break;
        total += r;
    }

    if (total != len) {
        throw StorageException(fmt::format("file {}. Read error : {}", filename_, strerror(errno)));
    }

}

void
File::Write(size_t addr, void* buffer, size_t len) {
    ScopedSpinLock lock(mutex_);
    ssize_t s;
    size_t total = 0;

    while (total < len) {
        s = pwrite(fd_, buffer, len, addr + total);
        if (s < 0) {
            throw StorageException(fmt::format("file {}. Write error : {}", filename_, strerror(errno)));
        }
        if (s == 0)
            break;
        total += s;
    }
}

void
File::Truncate(uint64_t newsize) {
    if (ftruncate(fd_, newsize))
        throw StorageException(fmt::format("file {}. Truncate error : {}", filename_, strerror(errno)));
    file_size_ = newsize;
}

uint64_t
File::Alloc(size_t requested_length) {
    ScopedSpinLock lock(mutex_);
    uint64_t address;

    if (excess_at_end_ >= requested_length) {
        address = file_size_ - excess_at_end_;
        excess_at_end_ -= requested_length;
    } else {
        uint64_t excess = 0;

        // If the file is large enough then allocate more space to avoid
        // frequent calls to ftruncate(); these calls cause bad performance
        // spikes.
        //
        if (file_size_ < requested_length * 100)
            excess = 0;
        else if (file_size_ < requested_length * 250)
            excess = requested_length * 100;
        else if (file_size_ < requested_length * 1000)
            excess = requested_length * 250;
        else
            excess = requested_length * 1000;

        address = file_size_;
        Truncate(address + requested_length + excess);
        excess_at_end_ = excess;
    }
    return address;
}

void
File::AllocPage(Page *page) {
    ScopedSpinLock lock(mutex_);
    uint64_t address = Alloc(Page::kSize);
    page->SetAddress(address);

    // allocate a memory buffer
    void *p = std::aligned_alloc(Page::kSize, Page::kSize);
    page->AssignAllocatedBuffer(p, address);
}

void
File::FreePage(Page *page) {
    ScopedSpinLock lock(mutex_);
    page->FreeBuffer();
}

void
File::ReadPage(Page *page, uint64_t address) {
    ScopedSpinLock lock(mutex_);
    if (page->Data() == 0) {
        // note that |p| will not leak if file.pread() throws; |p| is stored
        // in the |page| object and will be cleaned up by the caller in
        // case of an exception.
        void *p = std::aligned_alloc(Page::kSize, Page::kSize);
        page->AssignAllocatedBuffer(p, address);
    }
    Read(address, page->Data(), Page::kSize);
}

void
File::ReclaimSpace() {
    ScopedSpinLock lock(mutex_);
    if (excess_at_end_ > 0) {
        Truncate(file_size_ - excess_at_end_);
        excess_at_end_ = 0;
    }
}
}