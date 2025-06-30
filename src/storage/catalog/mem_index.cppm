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

export module mem_index;

import stl;

namespace infinity {

class BaseMemIndex;
class HnswIndexInMem;
class IVFIndexInMem;
class MemoryIndexer;
class SecondaryIndexInMem;
class EMVBIndexInMem;
class BMPIndexInMem;
class DummyIndexInMem;

export struct MemIndexID {
    String db_name_;
    String table_name_;
    String index_name_;
    SegmentID segment_id_ = 0;
};

export struct MemIndex {
    ~MemIndex();

    void ClearMemIndex();
    SizeT GetMemUsed() const;
    RowID GetBeginRowID();
    SizeT GetRowCount();

    BaseMemIndex *GetBaseMemIndex(const MemIndexID &mem_index_id);
    const BaseMemIndex *GetBaseMemIndex() const;
    void SetBaseMemIndexInfo(const String &db_name, const String &table_name, const String &index_name, const SegmentID &segment_id);

    void SetEMVBMemIndexInfo(const String &db_name, const String &table_name, const String &index_name, const SegmentID &segment_id);
    EMVBIndexInMem *GetEMVBMemIndex(const MemIndexID &mem_index_id);

    SharedPtr<HnswIndexInMem> GetHnswIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_hnsw_index_;
    }
    SharedPtr<IVFIndexInMem> GetIVFIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_ivf_index_;
    }
    SharedPtr<MemoryIndexer> GetFulltextIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_indexer_;
    }
    SharedPtr<SecondaryIndexInMem> GetSecondaryIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_secondary_index_;
    }
    SharedPtr<EMVBIndexInMem> GetEMVBIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_emvb_index_;
    }

    SharedPtr<BMPIndexInMem> GetBMPIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_bmp_index_;
    }

    SharedPtr<DummyIndexInMem> GetDummyIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        return memory_dummy_index_;
    }

    mutable std::mutex mtx_; // Used by append / mem index dump / clear

    SharedPtr<HnswIndexInMem> memory_hnsw_index_{};
    SharedPtr<IVFIndexInMem> memory_ivf_index_{};
    SharedPtr<MemoryIndexer> memory_indexer_{};
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_{};
    SharedPtr<EMVBIndexInMem> memory_emvb_index_{};
    SharedPtr<BMPIndexInMem> memory_bmp_index_{};
    SharedPtr<DummyIndexInMem> memory_dummy_index_{};
};

} // namespace infinity