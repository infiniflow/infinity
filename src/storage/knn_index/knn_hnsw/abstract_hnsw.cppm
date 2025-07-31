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
struct ChunkIndexMetaInfo;
}

export module abstract_hnsw;

import stl;
import hnsw_alg;
import data_store;
import vec_store_type;
import dist_func_l2;
import dist_func_ip;
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
import buffer_handle;
import third_party;

namespace infinity {

class BufferManager;
class ColumnVector;
class BufferObj;

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
                                         KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset> *,
                                         KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset> *,
                                         KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset> *,

                                         KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset, false> *,
                                         KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset, false> *,
                                         KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset, false> *,
                                         KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset, false> *,
                                         KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset, false> *,
                                         KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset, false> *,
                                         KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset, false> *,
                                         std::nullptr_t>;
export struct HnswIndexInMem : public BaseMemIndex {
public:
    HnswIndexInMem() : hnsw_(nullptr) {}

    static UniquePtr<HnswIndexInMem> Make(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, bool trace = false);

    static UniquePtr<HnswIndexInMem> Make(const IndexBase *index_base, const ColumnDef *column_def, bool trace = false);

    HnswIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, bool trace);

private:
    template <typename Iter, typename Index>
    static void InsertVecs(Index &index, Iter &&iter, const HnswInsertConfig &config, SizeT &mem_usage) {
        auto &thread_pool = InfinityContext::instance().GetHnswBuildThreadPool();
        if (thread_pool.size() == 0) {
            LOG_CRITICAL(fmt::format("Dense index building worker: {}", InfinityContext::instance().config()->DenseIndexBuildingWorker()));
            UnrecoverableError("Hnsw build thread pool size is 0, config.");
        }
        using T = std::decay_t<decltype(index)>;
        if constexpr (!std::is_same_v<T, std::nullptr_t>) {
            using IndexT = std::decay_t<decltype(*index)>;
            if constexpr (!IndexT::kOwnMem) {
                UnrecoverableError("HnswIndexInMem::InsertVecs: index does not own memory");
            } else {
                SizeT mem1 = index->mem_usage();
                auto [start, end] = index->StoreData(std::forward<Iter>(iter), config);
                SizeT bucket_size = std::max(kBuildBucketSize, SizeT(end - start - 1) / thread_pool.size() + 1);
                SizeT bucket_n = (end - start - 1) / bucket_size + 1;

                Vector<std::future<void>> futs;
                futs.reserve(bucket_n);
                for (SizeT i = 0; i < bucket_n; ++i) {
                    SizeT i1 = start + i * bucket_size;
                    SizeT i2 = std::min(i1 + bucket_size, SizeT(end));
                    futs.emplace_back(thread_pool.push([&index, i1, i2](int id) {
                        for (SizeT j = i1; j < i2; ++j) {
                            index->Build(j);
                        }
                    }));
                }
                for (auto &fut : futs) {
                    fut.get();
                }
                SizeT mem2 = index->mem_usage();
                mem_usage = mem2 - mem1;
            }
        }
    }

public:
    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    HnswIndexInMem(const HnswIndexInMem &) = delete;
    HnswIndexInMem &operator=(const HnswIndexInMem &) = delete;

    virtual ~HnswIndexInMem();

    SizeT GetRowCount() const;

    RowID GetBeginRowID() const override { return begin_row_id_; }

    SizeT GetSizeInBytes() const;

    void InsertVecs(SegmentOffset block_offset,
                    const ColumnVector &col,
                    BlockOffset offset,
                    BlockOffset row_count,
                    const HnswInsertConfig &config = kDefaultHnswInsertConfig);

    template <typename Iter>
    void InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig, bool trace = true) {
        SizeT mem_usage{};
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return;
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (!std::is_same_v<const typename IndexT::DataType *, typename Iter::ValueType>) {
                        UnrecoverableError("Data type mismatch.");
                    } else if constexpr (IndexT::kOwnMem) {
                        HnswIndexInMem::InsertVecs(index, std::move(iter), config, mem_usage);
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
        if (trace) {
            IncreaseMemoryUsageBase(mem_usage);
        }
    }

    void SetLSGParam(float alpha, float *avg);

    void Dump(BufferObj *buffer_obj, SizeT *dump_size = nullptr);

    const AbstractHnsw &get() const { return hnsw_; }

    AbstractHnsw *get_ptr() { return &hnsw_; }

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

protected:
    MemIndexTracerInfo GetInfo() const override;

private:
    static constexpr SizeT kBuildBucketSize = 1024;

    RowID begin_row_id_ = {};
    AbstractHnsw hnsw_ = nullptr;

    bool trace_{};
    bool own_memory_{};
    BufferHandle chunk_handle_{};
};

} // namespace infinity