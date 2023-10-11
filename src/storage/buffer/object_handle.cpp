#include "storage/buffer/object_handle.h"
#include "storage/buffer/buffer_handle.h"

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

SharedPtr<String> ObjectHandle::GetDir() const { return buffer_handle_->current_dir_; }

ptr_t ObjectHandle::GetData() {
    if (ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

} // namespace infinity