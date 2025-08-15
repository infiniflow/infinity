// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:base_memindex;

import :stl;
import :memindex_tracer;
import :chunk_index_meta;

import row_id;

namespace infinity {

export class BaseMemIndex : public std::enable_shared_from_this<BaseMemIndex> {
public:
    virtual ~BaseMemIndex() = default;

    virtual MemIndexTracerInfo GetInfo() const = 0;

    virtual const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const = 0;

    virtual RowID GetBeginRowID() const = 0;

    size_t GetMemUsed() const;

    size_t GetRowCount() const;

protected:
    void IncreaseMemoryUsageBase(size_t mem);
    void DecreaseMemoryUsageBase(size_t mem);

public:
    std::string db_name_;
    std::string table_name_;
    std::string index_name_;
    SegmentID segment_id_ = 0;
};

// Only for test
export class DummyIndexInMem : public BaseMemIndex {
public:
    DummyIndexInMem(std::string db_name, std::string table_name, std::string index_name, SegmentID segment_id, MemIndexTracer *tracer)
        : db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)), segment_id_(segment_id),
          tracer_(tracer) {}

    ~DummyIndexInMem() {
        if (tracer_ != nullptr) {
            tracer_->DecreaseMemUsed(mem_used_);
        }
    }

    void Append(size_t row_cnt, size_t mem_used) {
        {
            std::lock_guard lck(mtx_);
            row_cnt_ += row_cnt;
            mem_used_ += mem_used;
        }
        if (tracer_ != nullptr) {
            tracer_->IncreaseMemoryUsage(mem_used);
        }
    }

    void Dump() {}

    MemIndexTracerInfo GetInfo() const override {
        std::lock_guard lck(mtx_);
        return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                                  std::make_shared<std::string>(table_name_),
                                  std::make_shared<std::string>(db_name_),
                                  mem_used_,
                                  row_cnt_);
    }

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override {
        std::lock_guard lck(mtx_);
        return ChunkIndexMetaInfo{"chunk1", RowID(segment_id_, 0), row_cnt_, mem_used_};
    }

    RowID GetBeginRowID() const override { return RowID{segment_id_, 0}; }

public:
    std::string db_name_;
    std::string table_name_;
    std::string index_name_;
    SegmentID segment_id_{};
    MemIndexTracer *tracer_{nullptr};

    mutable std::mutex mtx_; // protect row_cnt_, mem_used_;
    size_t row_cnt_{};
    size_t mem_used_{};
};

} // namespace infinity
