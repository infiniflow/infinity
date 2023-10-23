module;

#include "faiss/impl/io.h"
#include "faiss/index_io.h"

import buffer_handle;
import stl;

module object_handle;

namespace infinity {

ObjectHandle::ObjectHandle(BufferHandle *buffer_handle) : buffer_handle_(buffer_handle) {}

// ObjectHandle::ObjectHandle(const ObjectHandle &other) : buffer_handle_(other.buffer_handle_), ptr_((other.ptr_)) {
//     buffer_handle_->AddRefCount();
// }

ObjectHandle::ObjectHandle(ObjectHandle &&other) noexcept : buffer_handle_(other.buffer_handle_) { other.buffer_handle_ = nullptr; }

// ObjectHandle& ObjectHandle::operator=(const ObjectHandle &other) {
//     if (buffer_handle_) {
//         buffer_handle_->UnloadData();
//     }
//     ptr_ = other.ptr_;
//     buffer_handle_ = other.buffer_handle_;
//     if (buffer_handle_) {
//         buffer_handle_->AddRefCount();
//     }
//     return *this;
// }

ObjectHandle &ObjectHandle::operator=(ObjectHandle &&other) noexcept {
    if (buffer_handle_) {
        buffer_handle_->UnloadData();
    }
    // ptr_ = other.ptr_;
    buffer_handle_ = other.buffer_handle_;
    // other.ptr_ = nullptr;
    other.buffer_handle_ = nullptr;
    return *this;
}

ObjectHandle::~ObjectHandle() {}

SharedPtr<String> ObjectHandle::GetDir() const {
    return buffer_handle_->current_dir_; // TODO shenyushi: check if shared_ptr is needed here
}

//---------------------------------------------------------------------------------------------

CommonObjectHandle::CommonObjectHandle(BufferHandle *buffer_handle) : ObjectHandle(buffer_handle) {}

CommonObjectHandle::~CommonObjectHandle() {
    if (ptr_ != nullptr && buffer_handle_ != nullptr) {
        buffer_handle_->UnloadData();
    }
}

CommonObjectHandle::CommonObjectHandle(CommonObjectHandle &&other) : ObjectHandle(Move(other)), ptr_(other.ptr_) { other.ptr_ = nullptr; }

CommonObjectHandle &CommonObjectHandle::operator=(CommonObjectHandle &&other) {
    ObjectHandle::operator=(Move(other));
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
    return *this;
}

ptr_t CommonObjectHandle::GetData() {
    if (ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

//---------------------------------------------------------------------------------------------
IndexObjectHandle::IndexObjectHandle(BufferHandle *buffer_handle) : ObjectHandle(buffer_handle) {}

IndexObjectHandle::IndexObjectHandle(IndexObjectHandle &&other) : ObjectHandle(Move(other)), index_(other.index_) { other.index_ = nullptr; }

IndexObjectHandle &IndexObjectHandle::operator=(IndexObjectHandle &&other) {
    ObjectHandle::operator=(Move(other));
    index_ = other.index_;
    other.index_ = nullptr;
    return *this;
}

// Implement the interface of faiss::IOReader.
// The reader is used to load index from buffer.
// The buffer is loaded from disk before is constructed. And its lifetime is controlled by BufferHandle.
struct BufferIOReader : faiss::IOReader {
    BufferIOReader(ptr_t ptr, size_t buffer_size) : ptr_(ptr), buffer_size_(buffer_size) {}

    size_t operator()(void *ptr, size_t size, size_t nitems) override {
        size_t read_n = size * nitems;
        if (read_n == 0) {
            return 0;
        }
        char *dst_ptr = (char *)ptr;
        read_n = std::min(read_n, buffer_size_ - rp_);
        Memcpy(dst_ptr, ptr_ + rp_, read_n);
        rp_ += read_n;
        return read_n / size;
    }

private:
    const size_t buffer_size_;
    const ptr_t ptr_;
    size_t rp_ = 0; // current read position
};

faiss::Index *IndexObjectHandle::GetIndex() {
    if (index_ == nullptr) {
        ptr_t ptr = buffer_handle_->LoadData();
        BufferIOReader reader(ptr, buffer_handle_->buffer_size_);
        index_ = read_index(&reader, 0);
    }
    return index_;
}

} // namespace infinity