#include "page.h"
#include "page_io.h"
#include "btree_node_proxy.h"

#include <cstdint>
#include <limits>


namespace infinity {

Page::Page(File *file):file_(file) {
    persisted_data_.raw_data_ = 0;
    persisted_data_.is_dirty_ = false;
    persisted_data_.is_allocated_ = false;
    persisted_data_.address_  = 0;
    persisted_data_.size_     = kSize;
}

Page::~Page() {
    FreeBuffer();
}

void Page::FreeBuffer() {
    if (node_proxy_) {
        delete node_proxy_;
        node_proxy_ = 0;
    }
}

void Page::Alloc(uint32_t type, uint32_t flags) {
    file_->AllocPage(this);
    if (flags & kInitializeWithZeroes) {
        memset(RawPayload(), 0, kSize);
    }
    if (type)
        SetType(type);
}

void Page::Fetch(uint64_t address) {
    file_->ReadPage(this, address);
    SetAddress(address);
}

void Page::Flush() {
    if (persisted_data_.is_dirty_) {
        /*update crc32
        if (likely(!persisted_data_.is_without_header_)) {
          MurmurHash3_x86_32(persisted_data_.raw_data_->header_.payload_,
                             persisted_data_.size_ - (sizeof(PageHeader) - 1),
                             (uint32_t)persisted_data_.address_,
                             &persisted_data_.raw_data_->header_.crc32_);
        }*/
        file_->Write(persisted_data_.address_, persisted_data_.raw_data_,
                     persisted_data_.size_);
        persisted_data_.is_dirty_ = false;
        ms_page_count_flushed_++;
    }
}

}
