// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import buffer_handle;
import buffer_obj;

module buffer_handle;

namespace infinity {
BufferHandle::BufferHandle(BufferObj *buffer_obj, void *data) : buffer_obj_(buffer_obj), data_(data) {}

BufferHandle::BufferHandle(const BufferHandle &other) : buffer_obj_(other.buffer_obj_), data_(other.data_) { ++buffer_obj_->rc_; }

BufferHandle &BufferHandle::operator=(const BufferHandle &other) {
    if (buffer_obj_) {
        buffer_obj_->UnloadInner();
    }
    buffer_obj_ = other.buffer_obj_;
    data_ = other.data_;
    ++buffer_obj_->rc_;
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

const void *BufferHandle::GetData() const { return data_; }

void *BufferHandle::GetDataMut() {
    buffer_obj_->GetMutPointer();
    return data_;
}
} // namespace infinity