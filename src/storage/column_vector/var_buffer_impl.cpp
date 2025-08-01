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

#include <vector>

module infinity_core:var_buffer.impl;

import :var_buffer;
import :stl;
import :infinity_exception;
import :third_party;
import :buffer_manager;
import :var_file_worker;
import :logger;
import :infinity_context;

namespace infinity {

SizeT VarBuffer::Append(UniquePtr<char[]> buffer, SizeT size, bool *free_success_p) {
    if (std::holds_alternative<const char *>(buffers_)) {
        UnrecoverableError("Cannot append to a const buffer");
    }
    auto &buffers = std::get<Vector<UniquePtr<char[]>>>(buffers_);
    std::unique_lock lock(mtx_);
    buffers.push_back(std::move(buffer));
    SizeT offset = buffer_size_prefix_sum_.back();
    buffer_size_prefix_sum_.push_back(offset + size);

    bool free_success = true;
    if (buffer_obj_ != nullptr) {
        free_success = buffer_obj_->AddBufferSize(size);
    }
    if (free_success_p != nullptr) {
        *free_success_p = free_success;
    }
    return offset;
}

SizeT VarBuffer::Append(const char *data, SizeT size, bool *free_success) {
    auto buffer = MakeUnique<char[]>(size);
    std::memcpy(buffer.get(), data, size);
    return Append(std::move(buffer), size, free_success);
}

const char *VarBuffer::Get(SizeT offset, SizeT size) const {
    if (size == 0) {
        return nullptr;
    }
    if (std::holds_alternative<const char *>(buffers_)) {
        const auto *buffer = std::get<const char *>(buffers_);
        return buffer + offset;
    }
    auto &buffers = std::get<Vector<UniquePtr<char[]>>>(buffers_);
    std::shared_lock lock(mtx_);
    // find the last index i such that buffer_size_prefix_sum_[i] <= offset
    auto it = std::upper_bound(buffer_size_prefix_sum_.begin(), buffer_size_prefix_sum_.end(), offset);
    if (it == buffer_size_prefix_sum_.end()) {
        String error_msg = fmt::format("offset {} is out of range {}", offset, buffer_size_prefix_sum_.back());
        UnrecoverableError(error_msg);
    }
    if (it == buffer_size_prefix_sum_.begin()) {
        String error_msg = fmt::format("prefix_sum[0] should be 0, but got {}", *it);
        UnrecoverableError(error_msg);
    }
    SizeT i = std::distance(buffer_size_prefix_sum_.begin(), it) - 1;
    SizeT offset_in_buffer = offset - buffer_size_prefix_sum_[i];
    if (offset_in_buffer + size > buffer_size_prefix_sum_[i + 1]) {
        String error_msg =
            fmt::format("offset {} and size {} is out of range [{}, {})", offset, size, buffer_size_prefix_sum_[i], buffer_size_prefix_sum_[i + 1]);
        UnrecoverableError(error_msg);
    }
    return buffers[i].get() + offset_in_buffer;
}

SizeT VarBuffer::Write(char *ptr) const {
    if (std::holds_alternative<const char *>(buffers_)) {
        UnrecoverableError("Cannot write to a const buffer");
        // const auto *buffer = std::get<const char *>(buffers_);
        // std::memcpy(ptr, buffer, buffer_size_prefix_sum_.back());
        // return buffer_size_prefix_sum_.back();
    }
    auto &buffers = std::get<Vector<UniquePtr<char[]>>>(buffers_);
    std::shared_lock lock(mtx_);
    char *start = ptr;
    for (SizeT i = 0; i < buffers.size(); ++i) {
        const auto &buffer = buffers[i];
        SizeT buffer_size = buffer_size_prefix_sum_[i + 1] - buffer_size_prefix_sum_[i];
        std::memcpy(ptr, buffer.get(), buffer_size);
        ptr += buffer_size;
    }
    return ptr - start;
}

SizeT VarBuffer::Write(char *ptr, SizeT offset, SizeT size) const {
    std::shared_lock lock(mtx_);
    const char *data = Get(offset, size);
    std::memcpy(ptr, data, size);
    ptr += size;
    return size;
}

SizeT VarBuffer::TotalSize() const {
    std::shared_lock lock(mtx_);
    return buffer_size_prefix_sum_.back();
}

SizeT VarBufferManager::Append(UniquePtr<char[]> data, SizeT size, bool *free_success) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto *buffer = GetInnerMutNoLock();
    SizeT offset = buffer->Append(std::move(data), size, free_success);
    return offset;
}

VarBufferManager::VarBufferManager(BufferObj *outline_buffer_obj)
    : type_(BufferType::kNewCatalog), buffer_handle_(None), outline_buffer_obj_(outline_buffer_obj) {}

SizeT VarBufferManager::Append(const char *data, SizeT size, bool *free_success) {
    auto buffer = MakeUnique<char[]>(size);
    std::memcpy(buffer.get(), data, size);
    return Append(std::move(buffer), size, free_success);
}

void VarBufferManager::SetToCatalog(BufferObj *outline_buffer_obj) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (type_ != BufferType::kBuffer) {
        UnrecoverableError("Cannot convert to new catalog");
    }
    type_ = BufferType::kNewCatalog;
    outline_buffer_obj_ = outline_buffer_obj;
    if (!mem_buffer_) {
        mem_buffer_ = MakeUnique<VarBuffer>();
    }
    outline_buffer_obj_->SetData(mem_buffer_.release());

    buffer_handle_ = outline_buffer_obj_->Load();
}

VarBuffer *VarBufferManager::GetInnerMutNoLock() {
    switch (type_) {
        case BufferType::kBuffer: {
            if (mem_buffer_.get() == nullptr) {
                mem_buffer_ = MakeUnique<VarBuffer>();
            }
            return mem_buffer_.get();
        }
        case BufferType::kNewCatalog: {
            if (!buffer_handle_.has_value()) {
                buffer_handle_ = outline_buffer_obj_->Load();
            }
            return static_cast<VarBuffer *>(buffer_handle_->GetDataMut());
        }
    }
}

const VarBuffer *VarBufferManager::GetInnerNoLock() {
    switch (type_) {
        case BufferType::kBuffer: {
            if (mem_buffer_.get() == nullptr) {
                mem_buffer_ = MakeUnique<VarBuffer>();
            }
            return mem_buffer_.get();
        }
        case BufferType::kNewCatalog: {
            if (!buffer_handle_.has_value()) {
                buffer_handle_ = outline_buffer_obj_->Load();
            }
            return static_cast<const VarBuffer *>(buffer_handle_->GetData());
        }
    }
}

const char *VarBufferManager::Get(SizeT offset, SizeT size) {
    std::unique_lock<std::mutex> lock(mutex_);
    return GetInnerNoLock()->Get(offset, size);
}

SizeT VarBufferManager::Write(char *ptr) {
    std::unique_lock<std::mutex> lock(mutex_);
    return GetInnerNoLock()->Write(ptr);
}

SizeT VarBufferManager::Write(char *ptr, SizeT offset, SizeT size) {
    std::unique_lock<std::mutex> lock(mutex_);
    return GetInnerNoLock()->Write(ptr, offset, size);
}

SizeT VarBufferManager::TotalSize() {
    std::unique_lock<std::mutex> lock(mutex_);
    return GetInnerNoLock()->TotalSize();
}

} // namespace infinity
