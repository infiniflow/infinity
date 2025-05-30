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

module mem_index;

import stl;
import base_memindex;
import secondary_index_in_mem;
import ivf_index_data_in_mem;
import emvb_index_in_mem;
import memory_indexer;
#ifdef INDEX_HANDLER
import hnsw_handler;
import bmp_handler;
#else
import abstract_hnsw;
import abstract_bmp;
#endif

namespace infinity {

MemIndex::~MemIndex() = default;

void MemIndex::ClearMemIndex() {
    std::unique_lock<std::mutex> lock(mtx_);

    memory_hnsw_index_.reset();
    memory_ivf_index_.reset();
    memory_indexer_.reset();
    memory_secondary_index_.reset();
    memory_emvb_index_.reset();
    memory_bmp_index_.reset();
}

BaseMemIndex *MemIndex::GetBaseMemIndex(const MemIndexID &mem_index_id) {
    BaseMemIndex *res = nullptr;
    if (memory_hnsw_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_hnsw_index_.get());
    } else if (memory_ivf_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_ivf_index_.get());
    } else if (memory_indexer_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_indexer_.get());
    } else if (memory_secondary_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_secondary_index_.get());
    } else if (memory_bmp_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_bmp_index_.get());
    } else {
        return nullptr;
    }
    res->db_name_ = mem_index_id.db_name_;
    res->table_name_ = mem_index_id.table_name_;
    res->index_name_ = mem_index_id.index_name_;
    res->segment_id_ = mem_index_id.segment_id_;
    return res;
}

const BaseMemIndex *MemIndex::GetBaseMemIndex() const{
    std::unique_lock<std::mutex> lock(mtx_);
    BaseMemIndex *res = nullptr;
    if (memory_hnsw_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_hnsw_index_.get());
    } else if (memory_ivf_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_ivf_index_.get());
    } else if (memory_indexer_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_indexer_.get());
    } else if (memory_secondary_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_secondary_index_.get());
    } else if (memory_bmp_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_bmp_index_.get());
    } else {
        return nullptr;
    }

    return res;
}

} // namespace infinity