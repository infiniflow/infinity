#include "page_io.h"

namespace infinity {

File::File(const std::filesystem::path& name,
           size_t initial_num_pages)
    : fd_(-1),
      file_size_(0),
      growth_bytes_(kGrowthPages * Page::kSize),
      next_page_allocation_offset_(initial_num_pages * Page::kSize) {
    CHECK_ERROR(
        fd_ = open(name.c_str(),
                   O_CREAT | O_RDWR | O_SYNC |
#if defined(__linux__)
                   O_DIRECT
#else
                   0
#endif
                   ,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

    // Retrieve the file's size.
    struct stat file_status;
    CHECK_ERROR(fstat(fd_, &file_status));
    assert(file_status.st_size >= 0);
    file_size_ = file_status.st_size;

    // If file already existed, retrieve next_page_allocation_offset_.
    if (file_size_ > 0) {
        PageBuffer page_data = PageMemoryAllocator::Allocate(1);
        Page temp_page(page_data.get());
        next_page_allocation_offset_ = file_size_;
        for (size_t offset = 0; offset < file_size_; offset += Page::kSize) {
            ReadPage(offset, page_data.get());
            if (!temp_page.IsValid()) {
                next_page_allocation_offset_ = offset;
                break;
            }
        }
    }
}
void File::ExpandToIfNeeded(size_t offset) {
    if (file_size_ >= (offset + Page::kSize)) return;

    size_t bytes_to_add = 0;
    while ((file_size_ + bytes_to_add) < (offset + Page::kSize))
        bytes_to_add += growth_bytes_;

    CHECK_ERROR(fallocate(fd_, /*mode=*/0, /*offset=*/file_size_,
                          /*len=*/bytes_to_add));
    file_size_ += bytes_to_add;
}

PageIO::PageIO(std::filesystem::path db_path){
    file_ = std::make_unique<File>(db_path, 0);
}

Status PageIO::ReadPage(const PhysicalPageId physical_page_id, void* data) {
    const size_t byte_offset = physical_page_id.GetOffset() * Page::kSize;
    return file_->ReadPage(byte_offset, data);
}

Status PageIO::WritePage(const PhysicalPageId physical_page_id, void* data) {
    const size_t byte_offset = physical_page_id.GetOffset() * Page::kSize;
    return file_->WritePage(byte_offset, data);
}

PhysicalPageId PageIO::AllocatePage() {
    std::unique_lock<std::mutex> locker(page_allocation_mutex_);
    const size_t offset = file_->AllocatePage();
    ++total_pages_;
    return PhysicalPageId(0, offset / Page::kSize);
}
}