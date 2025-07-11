//
// Created by infiniflow on 25-5-13.
//

module;

#include <future>

export module infinity_core:hnsw_handler;

import :stl;
import :hnsw_alg;
import :data_store;
import :vec_store_type;
import :dist_func_l2;
import :dist_func_ip;
import :hnsw_common;
import column_def;
import :index_hnsw;
import :infinity_exception;
import :index_base;
import :logger;
import internal_types;
import embedding_info;
import :infinity_context;
import :logger;
import :base_memindex;
import :memindex_tracer;
import :buffer_handle;
import :third_party;
import :config;
import :abstract_hnsw;
import :chunk_index_meta;

namespace infinity {

class BufferManager;
struct ColumnVector;
class BufferObj;
class LocalFileHandle;

export using AbstractHnsw = std::variant<UniquePtr<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset>>,

                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset, false>>,
                                  UniquePtr<KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset, false>>,
                                  std::nullptr_t>;

export struct HnswHandler {
public:
    HnswHandler() : hnsw_(nullptr) {}
    virtual ~HnswHandler() {}
    HnswHandler(const HnswHandler &) = delete;
    HnswHandler &operator=(const HnswHandler &) = delete;

    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    HnswHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    static UniquePtr<HnswHandler> Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true);

    template <typename DistanceT, typename LabelT, typename Filter = NoneType, bool WithLock = true>
    Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>>
    SearchIndex(const auto &q, SizeT k, const Filter &filter, const KnnSearchOption &option = {}) const {
        Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>> res{};
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    using IndexT = std::decay_t<decltype(*index)>;
                    using QueryVecType = typename IndexT::QueryVecType;
                    res = index->template KnnSearch<Filter, WithLock>(reinterpret_cast<QueryVecType>(q), k, filter, option);
                }
            },
            hnsw_);
        return res;
    }

    template <typename DistanceT, typename LabelT, bool WithLock = true>
    Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>> SearchIndex(const auto *q, SizeT k, const KnnSearchOption &option = {}) const {
        Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>> res{};
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    using IndexT = std::decay_t<decltype(*index)>;
                    using QueryVecType = typename IndexT::QueryVecType;
                    res = index->template KnnSearch<WithLock>(reinterpret_cast<QueryVecType>(q), k, option);
                }
            },
            hnsw_);
        return res;
    }

private:
    template <typename Iter, typename Index>
    static void InsertVecs(Index &index, Iter &&iter, const HnswInsertConfig &config, SizeT &mem_usage, SizeT kBuildBucketSize = 1024) {
        auto &thread_pool = InfinityContext::instance().GetHnswBuildThreadPool();
        if (thread_pool.size() == 0) {
            LOG_CRITICAL(fmt::format("Dense index building worker: {}", InfinityContext::instance().config()->DenseIndexBuildingWorker()));
            UnrecoverableError("Hnsw build thread pool size is 0, config.");
        }
        using T = std::decay_t<decltype(index)>;
        if constexpr (!std::is_same_v<T, std::nullptr_t>) {
            using IndexT = std::decay_t<decltype(*index)>;
            if constexpr (!IndexT::kOwnMem) {
                UnrecoverableError("HnswHandler::InsertVecs: index does not own memory");
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
    SizeT InsertVecs(SegmentOffset block_offset,
                     const ColumnVector &col,
                     BlockOffset offset,
                     BlockOffset row_count,
                     const HnswInsertConfig &config = kDefaultHnswInsertConfig,
                     SizeT kBuildBucketSize = 1024);

    template <typename Iter>
    SizeT InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig, SizeT kBuildBucketSize = 1024) {
        SizeT mem_usage{};
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (!std::is_same_v<const typename IndexT::DataType *, typename Iter::ValueType>) {
                        UnrecoverableError("Data type mismatch.");
                    } else if constexpr (IndexT::kOwnMem) {
                        HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
        return mem_usage;
    }

    template <typename LabelT>
    Pair<VertexType, VertexType> StoreData(const auto *data, SizeT dim, SizeT vec_num, const HnswInsertConfig &option = kDefaultHnswInsertConfig) {
        Pair<VertexType, VertexType> res{};
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (IndexT::kOwnMem) {
                        using DataType = typename IndexT::DataType;
                        using QueryVecType = typename IndexT::QueryVecType;
                        auto iter = DenseVectorIter<DataType, LabelT>(reinterpret_cast<QueryVecType>(data), dim, vec_num);
                        res = index->StoreData(std::move(iter), option);
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
        return res;
    }
    template <typename LabelT>
    LabelT GetLabel(VertexType vertex_i) const {
        LabelT res{};
        std::visit(
            [&](auto &&index) {
                using IndexType = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    res = index->GetLabel(vertex_i);
                }
            },
            hnsw_);
        return res;
    }

    // get infomation from hnsw_ (hnsw_ create in compile)
    SizeT MemUsage() const;
    SizeT GetRowCount() const;
    SizeT GetSizeInBytes() const;
    Pair<SizeT, SizeT> GetInfo() const;
    void Check() const;

    // hnsw_ data operator
    void SetLSGParam(float alpha, UniquePtr<float[]> avg);
    void SaveToPtr(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    void LoadFromPtr(LocalFileHandle &file_handle, SizeT file_size);
    void LoadFromPtr(const char *ptr, SizeT size);
    void Build(VertexType vertex_i);
    void Optimize();
    void CompressToLVQ();

private:
    AbstractHnsw hnsw_ = nullptr;
};

export using HnswHandlerPtr = HnswHandler *;

export struct HnswIndexInMem : public BaseMemIndex {
public:
    HnswIndexInMem() : hnsw_handler_(nullptr) {}
    HnswIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, bool trace)
        : begin_row_id_(begin_row_id), hnsw_handler_(HnswHandler::Make(index_base, column_def).release()), trace_(trace), own_memory_(true) {}
    HnswIndexInMem(const HnswIndexInMem &) = delete;
    HnswIndexInMem &operator=(const HnswIndexInMem &) = delete;
    virtual ~HnswIndexInMem();

    static UniquePtr<HnswIndexInMem> Make(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, bool trace = false);

    static UniquePtr<HnswIndexInMem> Make(const IndexBase *index_base, const ColumnDef *column_def, bool trace = false);

    void InsertVecs(SegmentOffset block_offset,
                    const ColumnVector &col,
                    BlockOffset offset,
                    BlockOffset row_count,
                    const HnswInsertConfig &config = kDefaultHnswInsertConfig);

    template <typename Iter>
    void InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig, bool trace = true) {
        SizeT mem_usage = hnsw_handler_->InsertVecs(std::move(iter), config, kBuildBucketSize);
        if (trace) {
            IncreaseMemoryUsageBase(mem_usage);
        }
    }

    void Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr = nullptr);

    RowID GetBeginRowID() const override { return begin_row_id_; }
    const HnswHandlerPtr &get() const { return hnsw_handler_; }
    HnswHandlerPtr *get_ptr() { return &hnsw_handler_; }
    void SetLSGParam(float alpha, UniquePtr<float[]> avg);
    SizeT GetRowCount() const;
    SizeT GetSizeInBytes() const;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

protected:
    MemIndexTracerInfo GetInfo() const override;

private:
    static constexpr SizeT kBuildBucketSize = 1024;

    RowID begin_row_id_ = {};
    HnswHandlerPtr hnsw_handler_;
    bool trace_{};
    bool own_memory_{};
    BufferHandle chunk_handle_{};
};

} // namespace infinity