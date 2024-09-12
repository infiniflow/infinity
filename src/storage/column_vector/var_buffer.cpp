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

module var_buffer;

import stl;
import infinity_exception;
import third_party;
import block_column_entry;
import buffer_manager;
import var_file_worker;
import logger;
import infinity_context;

namespace infinity {

SizeT VarBuffer::Append(UniquePtr<char[]> buffer, SizeT size, bool *free_success_p) {
    std::unique_lock lock(mtx_);
    buffers_.push_back(std::move(buffer));
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
    return buffers_[i].get() + offset_in_buffer;
}

SizeT VarBuffer::Write(char *ptr) const {
    std::shared_lock lock(mtx_);
    char *start = ptr;
    for (SizeT i = 0; i < buffers_.size(); ++i) {
        const auto &buffer = buffers_[i];
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
    auto *buffer = GetInnerMut();
    SizeT offset = buffer->Append(std::move(data), size, free_success);
    if (type_ == BufferType::kBufferObj) {
        block_column_entry_->SetLastChunkOff(buffer->TotalSize());
    }
    return offset;
}

VarBufferManager::VarBufferManager(BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr)
    : type_(BufferType::kBufferObj), buffer_handle_(None), block_column_entry_(block_column_entry), buffer_mgr_(buffer_mgr) {}

SizeT VarBufferManager::Append(const char *data, SizeT size, bool *free_success) {
    auto buffer = MakeUnique<char[]>(size);
    std::memcpy(buffer.get(), data, size);
    return Append(std::move(buffer), size, free_success);
}

void VarBufferManager::InitBuffer() {
    if (type_ == BufferType::kBuffer) {
        if (mem_buffer_.get() == nullptr) {
            mem_buffer_ = MakeUnique<VarBuffer>();
        }
    } else {
        if (!buffer_handle_.has_value()) {
            if (auto *block_obj = block_column_entry_->GetOutlineBuffer(0); block_obj == nullptr) {
                auto file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_column_entry_->block_entry()->block_dir(),
                                                             block_column_entry_->OutlineFilename(0),
                                                             0
                                                             /*buffer_size*/);
                auto *buffer_obj = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
                block_column_entry_->AppendOutlineBuffer(buffer_obj);
                buffer_handle_ = buffer_obj->Load();
            } else {
                buffer_handle_ = block_obj->Load();
            }
        }
    }
}

VarBuffer *VarBufferManager::GetInnerMut() {
    InitBuffer();
    if (type_ == BufferType::kBuffer) {
        return mem_buffer_.get();
    } else {
        return static_cast<VarBuffer *>(buffer_handle_->GetDataMut());
    }
}

const VarBuffer *VarBufferManager::GetInner() {
    InitBuffer();
    if (type_ == BufferType::kBuffer) {
        return mem_buffer_.get();
    } else {
        return static_cast<const VarBuffer *>(buffer_handle_->GetData());
    }
}

} // namespace infinity
