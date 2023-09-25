//
// Created by jinhai on 23-7-21.
//

#include "object_handle.h"

namespace infinity {

// ObjectHandle::ObjectHandle(const ObjectHandle &other) : buffer_handle_(other.buffer_handle_), ptr_((other.ptr_)) {
//     buffer_handle_->AddRefCount();
// }

ObjectHandle::ObjectHandle(ObjectHandle &&other) : buffer_handle_(other.buffer_handle_), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
    other.buffer_handle_ = nullptr;
}


ObjectHandle::~ObjectHandle() {
    if (buffer_handle_) {
        buffer_handle_->UnloadData();
    }
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

ObjectHandle& ObjectHandle::operator=(ObjectHandle &&other) {
    if (buffer_handle_) {
        buffer_handle_->UnloadData();
    }
    ptr_ = other.ptr_;
    buffer_handle_ = other.buffer_handle_;
    other.ptr_ = nullptr;
    other.buffer_handle_ = nullptr;
    return *this;
}

ptr_t
ObjectHandle::GetData() {
    if(ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

}