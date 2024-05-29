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

module var_heap;

namespace infinity {
#if 0
Pair<u64, u64> VarHeapManager::Allocate(SizeT nbytes) {
    if (nbytes == 0) {
        UnrecoverableError(fmt::format("Attempt to allocate memory with size: {} as the string heap", nbytes));

    }

    u64 start_chunk_id = INITIAL_VECTOR_CHUNK_ID;
    u64 start_chunk_offset = INVALID_CHUNK_OFFSET;

    SizeT rest_nbytes = nbytes;

    if (current_chunk_idx_ == INITIAL_VECTOR_CHUNK_ID) {
        // First chunk
        start_chunk_id = 0;
        start_chunk_offset = 0;

        while (current_chunk_size_ < nbytes && current_chunk_size_ < MAX_VECTOR_CHUNK_SIZE) {
            current_chunk_size_ *= 2;
        }

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
        // Not first chunk
        start_chunk_id = current_chunk_idx_;
        start_chunk_offset = current_chunk_offset_;

        if (current_chunk_size_ < MAX_VECTOR_CHUNK_SIZE) {

            if (current_chunk_offset_ + nbytes > current_chunk_size_) {
                while (current_chunk_offset_ + nbytes > current_chunk_size_ && current_chunk_size_ < MAX_VECTOR_CHUNK_SIZE) {
                    current_chunk_size_ *= 2;
                }

                if (current_chunk_size_ > chunks_[current_chunk_idx_]->capacity_) {
                    UniquePtr<VectorHeapChunk> new_chunk = MakeUnique<VectorHeapChunk>(current_chunk_size_);
                    std::memcpy(new_chunk->ptr_, chunks_[current_chunk_idx_]->ptr_, current_chunk_offset_);
                    chunks_[current_chunk_idx_] = std::move(new_chunk);
                }

                if(current_chunk_offset_ + nbytes < current_chunk_size_) {
                    current_chunk_offset_ += nbytes;
                    nbytes = 0;
                    return {start_chunk_id, start_chunk_offset};
                }

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
        } else {
            // current_chunk_size_ == MAX_VECTOR_CHUNK_SIZE
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
    }
}

// return value: start chunk id & chunk offset
Pair<u64, u64> VarHeapManager::AppendToHeap(const char *data_ptr, SizeT nbytes) {
    auto [chunk_id, chunk_offset] = Allocate(nbytes);

    u64 start_chunk_id = chunk_id;
    u64 start_chunk_offset = chunk_offset;
    while (nbytes > 0) {
        char *start_ptr = chunks_[chunk_id]->ptr_ + chunk_offset;
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

    return {start_chunk_id, start_chunk_offset};
}

// Read #nbytes size of data from offset: #chunk_offset of chunk: #chunk_id to buffer: #buffer, Make sure the buffer has enough space to hold
// the size of data.
void VarHeapManager::ReadFromHeap(char *buffer, u64 chunk_id, u64 chunk_offset, SizeT nbytes) {
//    u64 current_chunk_remain_size = current_chunk_size_ - chunk_offset;
    while (nbytes > 0) {
        char *start_ptr = chunks_[chunk_id]->ptr_ + chunk_offset;
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

String VarHeapManager::Stats() const {
    std::stringstream ss;
    ss << "Chunk count: " << current_chunk_idx_ << ", Chunk size: " << current_chunk_size_ << ", Current Offset: " << current_chunk_offset_
       << ", Total size: " << total_size() << std::endl;
    return ss.str();
}
#endif
} // namespace infinity
