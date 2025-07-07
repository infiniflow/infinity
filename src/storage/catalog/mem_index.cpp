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

#include "type/complex/row_id.h"

module infinity_core;

import :stl;
import :base_memindex;
import :secondary_index_in_mem;
import :ivf_index_data_in_mem;
import :emvb_index_in_mem;
import :memory_indexer;
#ifdef INDEX_HANDLER
import :hnsw_handler;
import :bmp_handler;
#else
import :abstract_hnsw;
import :abstract_bmp;
#endif

namespace infinity {

MemIndex::~MemIndex() = default;

SizeT MemIndex::GetMemUsed() const {
    const BaseMemIndex *base_mem_index = GetBaseMemIndex();
    if (base_mem_index == nullptr) {
        return 0;
    }
    return base_mem_index->GetMemUsed();
}

RowID MemIndex::GetBeginRowID() {
    const BaseMemIndex *base_mem_index = GetBaseMemIndex();
    if (base_mem_index != nullptr) {
        return base_mem_index->GetBeginRowID();
    }
    SharedPtr<EMVBIndexInMem> emvb_index = GetEMVBIndex();
    if (emvb_index != nullptr) {
        return emvb_index->GetBeginRowID();
    }
    return RowID();
}

SizeT MemIndex::GetRowCount() {
    const BaseMemIndex *base_mem_index = GetBaseMemIndex();
    if (base_mem_index != nullptr) {
        return base_mem_index->GetRowCount();
    }
    SharedPtr<EMVBIndexInMem> emvb_index = GetEMVBIndex();
    if (emvb_index != nullptr) {
        return emvb_index->GetRowCount();
    }
    return 0;
}

bool MemIndex::IsNull() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_hnsw_index_ == nullptr && memory_ivf_index_ == nullptr && memory_indexer_ == nullptr && memory_secondary_index_ == nullptr &&
           memory_emvb_index_ == nullptr && memory_bmp_index_ == nullptr && memory_dummy_index_ == nullptr;
}

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
    } else if (memory_dummy_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_dummy_index_.get());
    } else {
        return nullptr;
    }
    res->db_name_ = mem_index_id.db_name_;
    res->table_name_ = mem_index_id.table_name_;
    res->index_name_ = mem_index_id.index_name_;
    res->segment_id_ = mem_index_id.segment_id_;
    return res;
}

const BaseMemIndex *MemIndex::GetBaseMemIndex() const {
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
    } else if (memory_dummy_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_dummy_index_.get());
    } else {
        return nullptr;
    }

    return res;
}

void MemIndex::SetBaseMemIndexInfo(const String &db_name, const String &table_name, const String &index_name, const SegmentID &segment_id) {
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
    } else if (memory_dummy_index_.get() != nullptr) {
        res = static_cast<BaseMemIndex *>(memory_dummy_index_.get());
    } else {
        return;
    }
    res->db_name_ = db_name;
    res->table_name_ = table_name;
    res->index_name_ = index_name;
    res->segment_id_ = segment_id;
}

void MemIndex::SetEMVBMemIndexInfo(const String &db_name, const String &table_name, const String &index_name, const SegmentID &segment_id) {
    EMVBIndexInMem *res = memory_emvb_index_.get();
    if (res != nullptr) {
        res->db_name_ = db_name;
        res->table_name_ = table_name;
        res->index_name_ = index_name;
        res->segment_id_ = segment_id;
    }
}

EMVBIndexInMem *MemIndex::GetEMVBMemIndex(const MemIndexID &mem_index_id) {
    EMVBIndexInMem *res = memory_emvb_index_.get();
    if (res != nullptr) {
        res->db_name_ = mem_index_id.db_name_;
        res->table_name_ = mem_index_id.table_name_;
        res->index_name_ = mem_index_id.index_name_;
        res->segment_id_ = mem_index_id.segment_id_;
    }
    return res;
}

SharedPtr<HnswIndexInMem> MemIndex::GetHnswIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_hnsw_index_;
}

void MemIndex::SetHnswIndex(SharedPtr<HnswIndexInMem> hnsw_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_hnsw_index_ = hnsw_index;
}

SharedPtr<IVFIndexInMem> MemIndex::GetIVFIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_ivf_index_;
}

void MemIndex::SetIVFIndex(SharedPtr<IVFIndexInMem> ivf_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_ivf_index_ = ivf_index;
}

SharedPtr<MemoryIndexer> MemIndex::GetFulltextIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_indexer_;
}

void MemIndex::SetFulltextIndex(SharedPtr<MemoryIndexer> indexer) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_indexer_ = indexer;
}

SharedPtr<SecondaryIndexInMem> MemIndex::GetSecondaryIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_secondary_index_;
}

void MemIndex::SetSecondaryIndex(SharedPtr<SecondaryIndexInMem> secondary_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_secondary_index_ = secondary_index;
}

SharedPtr<EMVBIndexInMem> MemIndex::GetEMVBIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_emvb_index_;
}

void MemIndex::SetEMVBIndex(SharedPtr<EMVBIndexInMem> emvb_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_emvb_index_ = emvb_index;
}

SharedPtr<BMPIndexInMem> MemIndex::GetBMPIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_bmp_index_;
}

void MemIndex::SetBMPIndex(SharedPtr<BMPIndexInMem> bmp_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_bmp_index_ = bmp_index;
}

SharedPtr<DummyIndexInMem> MemIndex::GetDummyIndex() {
    std::unique_lock<std::mutex> lock(mtx_);
    return memory_dummy_index_;
}

void MemIndex::SetDummyIndex(SharedPtr<DummyIndexInMem> dummy_index) {
    std::unique_lock<std::mutex> lock(mtx_);
    memory_dummy_index_ = dummy_index;
}

} // namespace infinity