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

import stl;
import third_party;
import infinity_exception;
import default_values;
import vector_heap_chunk;

module fix_heap;

namespace infinity {

Pair<u64, u64> FixHeapManager::Allocate(SizeT nbytes) {
    if (nbytes == 0) {
        Error<ExecutorException>(fmt::format("Attempt to allocate memory with size: {} as the string heap", nbytes));
    }

    SizeT rest_nbytes = nbytes;
    u64 start_chunk_id = current_chunk_idx_;
    u64 start_chunk_offset = current_chunk_offset_;

    if (chunks_.empty())
        chunks_.emplace_back(MakeUnique<VectorHeapChunk>(current_chunk_size_));
    if (current_chunk_offset_ + nbytes > current_chunk_size_) {
        rest_nbytes -= (current_chunk_size_ - current_chunk_offset_);
        while (rest_nbytes > current_chunk_size_) {
            chunks_.emplace_back(MakeUnique<VectorHeapChunk>(current_chunk_size_));
            rest_nbytes -= current_chunk_size_;
        }

        if (rest_nbytes >= 0) {
            chunks_.emplace_back(MakeUnique<VectorHeapChunk>(current_chunk_size_));
        }

        current_chunk_offset_ = rest_nbytes;
        current_chunk_idx_ = chunks_.size() - 1;
        return {start_chunk_id, start_chunk_offset};
    } else {
        current_chunk_offset_ += nbytes;
        return {start_chunk_id, start_chunk_offset};
    }
}

// return value: start chunk id & chunk offset
Pair<u64, u64> FixHeapManager::AppendToHeap(const char *data_ptr, SizeT nbytes) {
    auto [chunk_id, chunk_offset] = Allocate(nbytes);

    u64 start_chunk_id = chunk_id;
    u64 start_chunk_offset = chunk_offset;
    while (nbytes > 0) {
        char *start_ptr = chunks_[chunk_id]->ptr_ + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        if (nbytes <= current_chunk_remain_size) {
            // Current chunk can hold the data
            Memcpy(start_ptr, data_ptr, nbytes);
            nbytes = 0;
        } else {
            Memcpy(start_ptr, data_ptr, current_chunk_remain_size);
            data_ptr += current_chunk_remain_size;
            nbytes -= current_chunk_remain_size;
            ++chunk_id;
            chunk_offset = 0;
        }
    }

    return {start_chunk_id, start_chunk_offset};
}

// return value: start chunk id & chunk offset
Pair<u64, u64> FixHeapManager::AppendToHeap(const FixHeapManager *src_heap_mgr, u64 src_chunk_id, u64 src_chunk_offset, SizeT nbytes) {
    auto [chunk_id, chunk_offset] = Allocate(nbytes);
    u64 start_chunk_id = chunk_id;
    u64 start_chunk_offset = chunk_offset;
    while (nbytes > 0) {
        char *start_ptr = chunks_[chunk_id]->ptr_ + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        SizeT src_chunk_remain_size = src_heap_mgr->current_chunk_size() - src_chunk_offset;

        SizeT copy_size{0};
        if (nbytes > src_chunk_remain_size) {
            // not all data will be copied in this chunk
            copy_size = Min(current_chunk_remain_size, src_chunk_remain_size);
        } else {
            copy_size = Min(current_chunk_remain_size, nbytes);
        }

        char *src_ptr = src_heap_mgr->chunks_[src_chunk_id]->ptr_ + src_chunk_offset;

        Memcpy(start_ptr, src_ptr, copy_size);

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

    return {start_chunk_id, start_chunk_offset};
}

// Read #nbytes size of data from offset: #chunk_offset of chunk: #chunk_id to buffer: #buffer, Make sure the buffer has enough space to hold
// the size of data.
void FixHeapManager::ReadFromHeap(char *buffer, u64 chunk_id, u64 chunk_offset, SizeT nbytes) {
    while (nbytes > 0) {
        char *start_ptr = chunks_[chunk_id]->ptr_ + chunk_offset;
        SizeT current_chunk_remain_size = current_chunk_size_ - chunk_offset;
        if (nbytes <= current_chunk_remain_size) {
            Memcpy(buffer, start_ptr, nbytes);
            nbytes = 0;
        } else {
            Memcpy(buffer, start_ptr, current_chunk_remain_size);
            buffer += current_chunk_remain_size;
            nbytes -= current_chunk_remain_size;
            ++chunk_id;
            chunk_offset = 0;
        }
    }
}

String FixHeapManager::Stats() const {
    std::stringstream ss;
    ss << "Chunk count: " << current_chunk_idx_ << ", Chunk size: " << current_chunk_size_ << ", Current Offset: " << current_chunk_offset_
       << ", Total size: " << total_size() << std::endl;
    return ss.str();
}

} // namespace infinity
