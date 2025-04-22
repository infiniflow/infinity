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

export struct MemIndexID {
    String db_name_;
    String table_name_;
    String index_name_;
    SegmentID segment_id_ = 0;
};

export struct MemIndex {
    ~MemIndex();

    void ClearMemIndex();

    BaseMemIndex *GetBaseMemIndex(const MemIndexID &mem_index_id);

    std::mutex mtx_; // Used by append / mem index dump / clear

    SharedPtr<HnswIndexInMem> memory_hnsw_index_{};
    SharedPtr<IVFIndexInMem> memory_ivf_index_{};
    SharedPtr<MemoryIndexer> memory_indexer_{};
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_{};
    SharedPtr<EMVBIndexInMem> memory_emvb_index_{};
    SharedPtr<BMPIndexInMem> memory_bmp_index_{};
};

} // namespace infinity