module;

import stl;
import buffer_handle;
import buffer_obj;

module buffer_handle;

namespace infinity {
BufferHandle::BufferHandle(BufferObj *buffer_obj, const void *data) : buffer_obj_(buffer_obj), data_(data) {}

BufferHandle::BufferHandle(const BufferHandle &other) : buffer_obj_(other.buffer_obj_), data_(other.data_) { buffer_obj_->rc_++; }

BufferHandle &BufferHandle::operator=(const BufferHandle &other) {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
    buffer_obj_ = other.buffer_obj_;
    data_ = other.data_;
    buffer_obj_->rc_++;
    return *this;
}

BufferHandle::BufferHandle(BufferHandle &&other) : buffer_obj_(other.buffer_obj_), data_(other.data_) {
    other.buffer_obj_ = nullptr;
    other.data_ = nullptr;
}

BufferHandle &BufferHandle::operator=(BufferHandle &&other) {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
    buffer_obj_ = other.buffer_obj_;
    data_ = other.data_;
    other.buffer_obj_ = nullptr;
    other.data_ = nullptr;
    return *this;
}

BufferHandle::~BufferHandle() {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
}

// --------------------------------------------------------------------------------------------

BufferHandleMut::BufferHandleMut(BufferObj *buffer_obj, void *data) : buffer_obj_(buffer_obj), data_(data) {}

BufferHandleMut::BufferHandleMut(BufferHandleMut &&other) : buffer_obj_(other.buffer_obj_), data_(other.data_) {
    other.buffer_obj_ = nullptr;
    other.data_ = nullptr;
}

BufferHandleMut &BufferHandleMut::operator=(BufferHandleMut &&other) {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
    buffer_obj_ = other.buffer_obj_;
    data_ = other.data_;
    other.buffer_obj_ = nullptr;
    other.data_ = nullptr;
    return *this;
}

BufferHandleMut::~BufferHandleMut() {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
}
} // namespace infinity