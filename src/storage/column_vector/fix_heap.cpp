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

#include <sstream>

module fix_heap;

import stl;
import third_party;
import infinity_exception;
import vector_heap_chunk;
import global_resource_usage;

import block_column_entry;
import buffer_manager;
import data_file_worker;
import logger;

namespace infinity {

FixHeapManager::FixHeapManager(const u32 heap_id, const u64 chunk_size, bool allow_cross_chunk) : heap_id_(heap_id), current_chunk_size_(chunk_size) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("FixHeapManager");
#endif
    current_chunk_idx_ = INVALID_CHUNK_ID;
    allow_storage_across_chunks_ = allow_cross_chunk;
}

FixHeapManager::FixHeapManager(const u32 heap_id,
                               BufferManager *buffer_mgr,
                               BlockColumnEntry *block_column_entry,
                               const u64 chunk_size,
                               bool allow_cross_chunk)
    : heap_id_(heap_id), current_chunk_size_(chunk_size), current_chunk_offset_(block_column_entry->LastChunkOff(heap_id)), buffer_mgr_(buffer_mgr),
      block_column_entry_(block_column_entry) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("FixHeapManager");
#endif
    const int cnt = block_column_entry->OutlineBufferCount(heap_id);
    if (cnt == 0) {
        current_chunk_idx_ = INVALID_CHUNK_ID;
    } else {
        current_chunk_idx_ = cnt - 1;
    }
    allow_storage_across_chunks_ = allow_cross_chunk;
}

FixHeapManager::~FixHeapManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("FixHeapManager");
#endif
    // std::variant in `VectorHeapChunk` will call destructor automatically
}

VectorHeapChunk FixHeapManager::AllocateChunk() {
    if (buffer_mgr_ == nullptr) {
        return VectorHeapChunk(current_chunk_size_);
    } else {
        // allocate by buffer_mgr, and store returned buffer_obj in `block_column_entry_`
        auto file_worker = MakeUnique<DataFileWorker>(block_column_entry_->FileDir(),
                                                      block_column_entry_->OutlineFilename(heap_id_, current_chunk_idx_),
                                                      current_chunk_size_);
        auto *buffer_obj = buffer_mgr_->AllocateBufferObject(std::move(file_worker));
        block_column_entry_->AppendOutlineBuffer(heap_id_, buffer_obj);
        return VectorHeapChunk(buffer_obj);
    }
}

Pair<ChunkId, u64> FixHeapManager::Allocate(SizeT nbytes) {
    if (nbytes == 0) {
        String error_message = fmt::format("Attempt to allocate memory with size: {} as the chunk heap", nbytes);
        UnrecoverableError(error_message);
    }

    SizeT rest_nbytes = nbytes;

    if (current_chunk_idx_ == INVALID_CHUNK_ID) {
        current_chunk_idx_ = 0;
        chunks_.emplace(current_chunk_idx_, AllocateChunk());
    }
    if (!allow_storage_across_chunks_) {
        if (nbytes > current_chunk_size_) {
            String error_message = fmt::format("Attempt to allocate memory with size: {}, but current chunk size: {}", nbytes, current_chunk_size_);
            UnrecoverableError(error_message);
        }
        if (current_chunk_offset_ + nbytes <= current_chunk_size_) {
            // use current chunk
            auto old_chunk_offset = current_chunk_offset_;
            current_chunk_offset_ += nbytes;
            return {current_chunk_idx_, old_chunk_offset};
        } else {
            // allocate new chunk
            chunks_.emplace(++current_chunk_idx_, AllocateChunk());
            current_chunk_offset_ = nbytes;
            return {current_chunk_idx_, 0};
        }
    }
    ChunkId start_chunk_id = current_chunk_idx_;
    ChunkId start_chunk_offset = current_chunk_offset_;
    if (current_chunk_offset_ + nbytes > current_chunk_size_) {
        rest_nbytes -= (current_chunk_size_ - current_chunk_offset_);
        while (rest_nbytes > current_chunk_size_) {
            chunks_.emplace(++current_chunk_idx_, AllocateChunk());
            rest_nbytes -= current_chunk_size_;
        }

        if (rest_nbytes >= 0) {
            chunks_.emplace(++current_chunk_idx_, AllocateChunk());
        }

        current_chunk_offset_ = rest_nbytes;
        return {start_chunk_id, start_chunk_offset};
    } else {
        current_chunk_offset_ += nbytes;
        return {start_chunk_id, start_chunk_offset};
    }
}

VectorHeapChunk &FixHeapManager::ReadChunk(ChunkId chunk_id) {
    if (auto iter = chunks_.find(chunk_id); iter != chunks_.end()) {
        return iter->second;
    }
    if (buffer_mgr_ == nullptr || chunk_id >= (ChunkId)block_column_entry_->OutlineBufferCount(heap_id_)) {
        String error_message = "No such chunk in heap";
        UnrecoverableError(error_message);
    }
    auto *outline_buffer = block_column_entry_->GetOutlineBuffer(heap_id_, chunk_id);
    if (outline_buffer == nullptr) {
        auto filename = block_column_entry_->OutlineFilename(heap_id_, chunk_id);
        auto file_worker = MakeUnique<DataFileWorker>(block_column_entry_->FileDir(), filename, current_chunk_size_);
        outline_buffer = buffer_mgr_->GetBufferObject(std::move(file_worker));

        if (outline_buffer == nullptr) {
            String error_message = "No such chunk in heap";
            UnrecoverableError(error_message);
        }
    }

    auto [iter, insert_ok] = chunks_.emplace(chunk_id, VectorHeapChunk(outline_buffer));
    return iter->second;
}

// return value: start chunk id & chunk offset
Pair<ChunkId, u64> FixHeapManager::AppendToHeap(const char *data_ptr, SizeT nbytes) {
    auto [chunk_id, chunk_offset] = Allocate(nbytes);

    ChunkId start_chunk_id = chunk_id;
    ChunkId start_chunk_offset = chunk_offset;
    while (nbytes > 0) {
        VectorHeapChunk &chunk = ReadChunk(chunk_id);
        char *start_ptr = chunk.GetPtrMut() + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        if (nbytes <= current_chunk_remain_size) {
            // Current chunk can hold the data
            std::memcpy(start_ptr, data_ptr, nbytes);
            nbytes = 0;
        } else {
            std::memcpy(start_ptr, data_ptr, current_chunk_remain_size);
            data_ptr += current_chunk_remain_size;
            nbytes -= current_chunk_remain_size;
            ++chunk_id;
            chunk_offset = 0;
        }
    }
    if (buffer_mgr_ != nullptr) {
        block_column_entry_->SetLastChunkOff(heap_id_, current_chunk_offset_);
    }

    return {start_chunk_id, start_chunk_offset};
}

Pair<ChunkID, u64> FixHeapManager::AppendToHeap(const Vector<Pair<const char *, SizeT>> &data_ptrs) {
    SizeT total_nbytes = 0;
    for (const auto &[data_ptr, nbytes] : data_ptrs) {
        total_nbytes += nbytes;
    }
    auto [chunk_id, chunk_offset] = Allocate(total_nbytes);

    ChunkId start_chunk_id = chunk_id;
    ChunkId start_chunk_offset = chunk_offset;
    for (auto [data_ptr, nbytes] : data_ptrs) {
        while (nbytes > 0) {
            VectorHeapChunk &chunk = ReadChunk(chunk_id);
            char *start_ptr = chunk.GetPtrMut() + chunk_offset;
            SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
            if (nbytes <= current_chunk_remain_size) {
                std::memcpy(start_ptr, data_ptr, nbytes);
                chunk_offset += nbytes;
                nbytes = 0;
            } else {
                std::memcpy(start_ptr, data_ptr, current_chunk_remain_size);
                data_ptr += current_chunk_remain_size;
                nbytes -= current_chunk_remain_size;
                ++chunk_id;
                chunk_offset = 0;
            }
        }
    }
    if (buffer_mgr_ != nullptr) {
        block_column_entry_->SetLastChunkOff(heap_id_, current_chunk_offset_);
    }

    return {start_chunk_id, start_chunk_offset};
}

// return value: start chunk id & chunk offset
Pair<ChunkId, u64> FixHeapManager::AppendToHeap(FixHeapManager *src_heap_mgr, ChunkId src_chunk_id, u64 src_chunk_offset, SizeT nbytes) {
    auto [chunk_id, chunk_offset] = Allocate(nbytes);
    ChunkId start_chunk_id = chunk_id;
    ChunkId start_chunk_offset = chunk_offset;
    while (nbytes > 0) {
        VectorHeapChunk &dst_chunk = ReadChunk(chunk_id);
        char *start_ptr = dst_chunk.GetPtrMut() + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        SizeT src_chunk_remain_size = src_heap_mgr->current_chunk_size() - src_chunk_offset;

        SizeT copy_size{0};
        if (nbytes > src_chunk_remain_size) {
            // not all data will be copied in this chunk
            copy_size = std::min(current_chunk_remain_size, src_chunk_remain_size);
        } else {
            copy_size = std::min(current_chunk_remain_size, nbytes);
        }

        const VectorHeapChunk &src_chunk = src_heap_mgr->ReadChunk(src_chunk_id);
        const char *src_ptr = src_chunk.GetPtr() + src_chunk_offset;

        std::memcpy(start_ptr, src_ptr, copy_size);

        current_chunk_remain_size -= copy_size;
        src_chunk_remain_size -= copy_size;
        nbytes -= copy_size;

        if (current_chunk_remain_size == 0) {
            ++chunk_id;
            chunk_offset = 0;
        } else {
            chunk_offset += copy_size;
        }

        if (src_chunk_remain_size == 0) {
            ++src_chunk_id;
            src_chunk_offset = 0;
        } else {
            src_chunk_offset += copy_size;
        }
    }
    if (buffer_mgr_ != nullptr) {
        block_column_entry_->SetLastChunkOff(heap_id_, current_chunk_offset_);
    }

    return {start_chunk_id, start_chunk_offset};
}

// Read #nbytes size of data from offset: #chunk_offset of chunk: #chunk_id to buffer: #buffer, Make sure the buffer has enough space to hold
// the size of data.
void FixHeapManager::ReadFromHeap(char *buffer, ChunkId chunk_id, u64 chunk_offset, SizeT nbytes) {
    while (nbytes > 0) {
        const VectorHeapChunk &src_chunk = ReadChunk(chunk_id);
        const char *start_ptr = src_chunk.GetPtr() + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        if (nbytes <= current_chunk_remain_size) {
            std::memcpy(buffer, start_ptr, nbytes);
            nbytes = 0;
        } else {
            std::memcpy(buffer, start_ptr, current_chunk_remain_size);
            buffer += current_chunk_remain_size;
            nbytes -= current_chunk_remain_size;
            ++chunk_id;
            chunk_offset = 0;
        }
    }
}

const char *FixHeapManager::GetRawPtrFromChunk(ChunkId chunk_id, u64 chunk_offset) {
    const VectorHeapChunk &src_chunk = ReadChunk(chunk_id);
    const char *start_ptr = src_chunk.GetPtr() + chunk_offset;
    return start_ptr;
}

String FixHeapManager::Stats() const {
    std::stringstream ss;
    ss << "Chunk count: " << current_chunk_idx_ << ", Chunk size: " << current_chunk_size_ << ", Current Offset: " << current_chunk_offset_
       << ", Total size: " << total_size() << std::endl;
    return ss.str();
}

// VarcharNextCharIterator::VarcharNextCharIterator(FixHeapManager *heap_mgr, const VarcharT &varchar) {
//     if (varchar.IsInlined()) {
//         data_ptr_ = varchar.short_.data_;
//         remain_size_ = varchar.length_;
//     } else {
//         heap_mgr_ = heap_mgr;
//         chunk_id_ = varchar.vector_.chunk_id_;
//         data_ptr_ = heap_mgr_->ReadChunk(chunk_id_).GetPtr() + varchar.vector_.chunk_offset_;
//         remain_size_ = heap_mgr_->current_chunk_size() - varchar.vector_.chunk_offset_;
//     }
// }

// [[nodiscard]] char VarcharNextCharIterator::GetNextChar() {
//     if (remain_size_ == 0) {
//         data_ptr_ = heap_mgr_->ReadChunk(++chunk_id_).GetPtr();
//         remain_size_ = heap_mgr_->current_chunk_size();
//     }
//     --remain_size_;
//     return *(data_ptr_++);
// }

// VarcharNextCharIterator FixHeapManager::GetNextCharIterator(const VarcharT &varchar) { return VarcharNextCharIterator(this, varchar); }

} // namespace infinity
