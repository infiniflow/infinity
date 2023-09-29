#include "object_handle.h"
#include "faiss/index_io.h"

namespace infinity {


ObjectHandle::ObjectHandle(BufferHandle* buffer_handle)
        : buffer_handle_(buffer_handle) {}

// ObjectHandle::ObjectHandle(const ObjectHandle &other) : buffer_handle_(other.buffer_handle_), ptr_((other.ptr_)) {
//     buffer_handle_->AddRefCount();
// }

ObjectHandle::ObjectHandle(ObjectHandle&& other) : buffer_handle_(other.buffer_handle_) {
    other.buffer_handle_ = nullptr;
}

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

ObjectHandle&
ObjectHandle::operator=(ObjectHandle&& other) {
    if(buffer_handle_) {
        buffer_handle_->UnloadData();
    }
    // ptr_ = other.ptr_;
    buffer_handle_ = other.buffer_handle_;
    // other.ptr_ = nullptr;
    other.buffer_handle_ = nullptr;
    return *this;
}

ObjectHandle::~ObjectHandle() {
    if(buffer_handle_) {
        buffer_handle_->UnloadData();
    }
}

SharedPtr<String>
ObjectHandle::GetDir() const {
    return buffer_handle_->current_dir_; // TODO shenyushi: check if shared_ptr is needed here
}

//---------------------------------------------------------------------------------------------


CommonObjectHandle::CommonObjectHandle(BufferHandle* buffer_handle)
        : ObjectHandle(buffer_handle) {}

CommonObjectHandle::CommonObjectHandle(CommonObjectHandle&& other)
        : ObjectHandle(std::move(other)), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

CommonObjectHandle&
CommonObjectHandle::operator=(CommonObjectHandle&& other) {
    ObjectHandle::operator=(std::move(other));
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
    return *this;
}

ptr_t
CommonObjectHandle::GetData() {
    if(ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

//---------------------------------------------------------------------------------------------

IndexObjectHandle::IndexObjectHandle(BufferHandle* buffer_handle)
        : ObjectHandle(buffer_handle) {}

IndexObjectHandle::IndexObjectHandle(IndexObjectHandle&& other)
        : ObjectHandle(std::move(other)), index_(other.index_) {
    other.index_ = nullptr;
}

IndexObjectHandle&
IndexObjectHandle::operator=(IndexObjectHandle&& other) {
    ObjectHandle::operator=(std::move(other));
    index_ = other.index_;
    other.index_ = nullptr;
    return *this;
}

faiss::Index*
IndexObjectHandle::GetIndex() {
    if(index_ == nullptr) {
        ptr_t _ptr = buffer_handle_->LoadData();
        int fd = buffer_handle_->fd_;
        FILE* file = fdopen(fd, "r"); // TODO shenyushi: check if this is correct
        index_ = faiss::read_index(file, 0); // TODO shenyushi: what does io_flags do
        fclose(file);
    }
    return index_;
}

}