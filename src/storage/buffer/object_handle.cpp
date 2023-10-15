#include "storage/buffer/object_handle.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/faiss_index_ptr.h"

namespace infinity {

ObjectHandle::ObjectHandle(BufferHandle *buffer_handle) : buffer_handle_(buffer_handle) {}

// TemplateHandle::TemplateHandle(const TemplateHandle &other) : buffer_handle_(other.buffer_handle_), ptr_((other.ptr_)) {
//     buffer_handle_->AddRefCount();
// }

ObjectHandle::ObjectHandle(ObjectHandle &&other) noexcept : buffer_handle_(other.buffer_handle_) { other.buffer_handle_ = nullptr; }

// TemplateHandle& TemplateHandle::operator=(const TemplateHandle &other) {
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

ObjectHandle::~ObjectHandle() {
    if (buffer_handle_) {
        buffer_handle_->UnloadData();
    }
}

SharedPtr<String> ObjectHandle::GetDir() const { return buffer_handle_->current_dir_; }

faiss::Index *ObjectHandle::GetIndex() { return static_cast<faiss::Index *>(GetRaw()); }

void ObjectHandle::WriteFaissIndex(FaissIndexPtr *index) {
    buffer_handle_->data_ = static_cast<void *>(index);
    ptr_ = static_cast<void *>(index);
}

void *ObjectHandle::GetRaw() {
    if (ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

} // namespace infinity