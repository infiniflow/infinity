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

export module infinity_core:mem_index;

import :infinity_type;

import std.compat;

import row_id;

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
    std::string db_name_;
    std::string table_name_;
    std::string index_name_;
    SegmentID segment_id_ = 0;
};

export class MemIndex {
public:
    ~MemIndex();

    bool IsNull() const;
    void ClearMemIndex();
    size_t GetMemUsed() const;
    RowID GetBeginRowID();
    size_t GetRowCount();

    BaseMemIndex *GetBaseMemIndex(const MemIndexID &mem_index_id);
    const BaseMemIndex *GetBaseMemIndex() const;
    void SetBaseMemIndexInfo(const std::string &db_name, const std::string &table_name, const std::string &index_name, const SegmentID &segment_id);

    void SetEMVBMemIndexInfo(const std::string &db_name, const std::string &table_name, const std::string &index_name, const SegmentID &segment_id);
    EMVBIndexInMem *GetEMVBMemIndex(const MemIndexID &mem_index_id);

    std::shared_ptr<HnswIndexInMem> GetHnswIndex();
    void SetHnswIndex(std::shared_ptr<HnswIndexInMem> hnsw_index);

    std::shared_ptr<IVFIndexInMem> GetIVFIndex();
    void SetIVFIndex(std::shared_ptr<IVFIndexInMem> ivf_index);

    std::shared_ptr<MemoryIndexer> GetFulltextIndex();
    void SetFulltextIndex(std::shared_ptr<MemoryIndexer> indexer);

    std::shared_ptr<SecondaryIndexInMem> GetSecondaryIndex();
    void SetSecondaryIndex(std::shared_ptr<SecondaryIndexInMem> secondary_index);

    std::shared_ptr<EMVBIndexInMem> GetEMVBIndex();
    void SetEMVBIndex(std::shared_ptr<EMVBIndexInMem> emvb_index);

    std::shared_ptr<BMPIndexInMem> GetBMPIndex();
    void SetBMPIndex(std::shared_ptr<BMPIndexInMem> bmp_index);

    std::shared_ptr<DummyIndexInMem> GetDummyIndex();
    void SetDummyIndex(std::shared_ptr<DummyIndexInMem> dummy_index);

    bool IsDumping() const;
    void SetIsDumping(bool is_dumping);
    void UpdateBegin();
    void UpdateEnd();
    void WaitUpdate();

private:
    mutable std::mutex mtx_; // Used by append / mem index dump / clear
    std::condition_variable cv_;
    bool is_dumping_{false};
    bool is_updating_{false};

    std::shared_ptr<HnswIndexInMem> memory_hnsw_index_{};
    std::shared_ptr<IVFIndexInMem> memory_ivf_index_{};
    std::shared_ptr<MemoryIndexer> memory_indexer_{};
    std::shared_ptr<SecondaryIndexInMem> memory_secondary_index_{};
    std::shared_ptr<EMVBIndexInMem> memory_emvb_index_{};
    std::shared_ptr<BMPIndexInMem> memory_bmp_index_{};
    std::shared_ptr<DummyIndexInMem> memory_dummy_index_{};
};

} // namespace infinity