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

#include <future>

namespace infinity {
struct SegmentEntry;
}

export module abstract_hnsw;

import stl;
import hnsw_alg;
import data_store;
import vec_store_type;
import dist_func_l2;
import dist_func_ip;
import file_system;
import hnsw_common;
import column_def;
import index_hnsw;
import infinity_exception;
import index_base;
import logger;
import internal_types;
import embedding_info;
import infinity_context;
import logger;
import base_memindex;
import memindex_tracer;

namespace infinity {

class BufferManager;
class ChunkIndexEntry;
class SegmentIndexEntry;
class BlockColumnEntry;

export using AbstractHnsw = std::variant<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset> *,
                                         KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset> *,
                                         KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset> *,
                                         KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset> *,
                                         KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset> *,
                                         std::nullptr_t>;

export struct HnswIndexInMem : public BaseMemIndex {
public:
    HnswIndexInMem() : hnsw_(nullptr) {}

    static UniquePtr<HnswIndexInMem>
    Make(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, SegmentIndexEntry *segment_index_entry, bool trace = false);

    HnswIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, SegmentIndexEntry *segment_index_entry, bool trace);

private:
    template <typename DataType>
    static AbstractHnsw InitAbstractIndex(const IndexHnsw *index_hnsw) {
        switch (index_hnsw->encode_type_) {
            case HnswEncodeType::kPlain: {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType>, SegmentOffset>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType>, SegmentOffset>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType>, SegmentOffset>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    default: {
                        return nullptr;
                    }
                }
            }
            case HnswEncodeType::kLVQ: {
                if constexpr (std::is_same_v<DataType, u8> || std::is_same_v<DataType, i8>) {
                    return nullptr;
                } else {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMetricL2: {
                            using HnswIndex = KnnHnsw<LVQL2VecStoreType<DataType, i8>, SegmentOffset>;
                            return static_cast<HnswIndex *>(nullptr);
                        }
                        case MetricType::kMetricInnerProduct: {
                            using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8>, SegmentOffset>;
                            return static_cast<HnswIndex *>(nullptr);
                        }
                        case MetricType::kMetricCosine: {
                            using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8>, SegmentOffset>;
                            return static_cast<HnswIndex *>(nullptr);
                        }
                        default: {
                            return nullptr;
                        }
                    }
                }
            }
            default: {
                return nullptr;
            }
        }
    }

    template <typename Iter, typename Index>
    static void InsertVecs(Index &index, Iter iter, const HnswInsertConfig &config, SizeT &mem_usage) {
        auto &thread_pool = InfinityContext::instance().GetHnswBuildThreadPool();
        using T = std::decay_t<decltype(index)>;
        if constexpr (!std::is_same_v<T, std::nullptr_t>) {
            SizeT mem1 = index->mem_usage();
            auto [start, end] = index->StoreData(std::move(iter), config);
            SizeT bucket_size = std::max(kBuildBucketSize, SizeT(end - start - 1) / thread_pool.size() + 1);
            SizeT bucket_n = (end - start - 1) / bucket_size + 1;

            Vector<std::future<void>> futs;
            futs.reserve(bucket_n);
            for (SizeT i = 0; i < bucket_n; ++i) {
                SizeT i1 = start + i * bucket_size;
                SizeT i2 = std::min(i1 + bucket_size, SizeT(end));
                futs.emplace_back(thread_pool.push([&, i1, i2](int id) {
                    for (SizeT j = i1; j < i2; ++j) {
                        index->Build(j);
                    }
                }));
            }
            for (auto &fut : futs) {
                fut.wait();
            }
            SizeT mem2 = index->mem_usage();
            mem_usage = mem2 - mem1;
        }
    }

public:
    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def);

    HnswIndexInMem(const HnswIndexInMem &) = delete;
    HnswIndexInMem &operator=(const HnswIndexInMem &) = delete;

    virtual ~HnswIndexInMem();

    SizeT GetRowCount() const;

    void InsertVecs(SizeT block_offset,
                    BlockColumnEntry *block_column_entry,
                    BufferManager *buffer_manager,
                    SizeT row_offset,
                    SizeT row_count,
                    const HnswInsertConfig &config = kDefaultHnswInsertConfig);

    void InsertVecs(const SegmentEntry *segment_entry,
                    BufferManager *buffer_mgr,
                    SizeT column_id,
                    TxnTimeStamp begin_ts,
                    bool check_ts,
                    const HnswInsertConfig &config = kDefaultHnswInsertConfig);

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr);

    const AbstractHnsw &get() const { return hnsw_; }

    AbstractHnsw *get_ptr() { return &hnsw_; }

protected:
    MemIndexTracerInfo GetInfo() const override;

private:
    static constexpr SizeT kBuildBucketSize = 1024;

    RowID begin_row_id_ = {};
    AbstractHnsw hnsw_ = nullptr;

    SegmentIndexEntry *segment_index_entry_{};
    bool trace_{};
};

} // namespace infinity