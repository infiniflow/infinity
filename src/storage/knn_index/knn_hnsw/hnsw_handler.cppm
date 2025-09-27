//
// Created by infiniflow on 25-5-13.
//

export module infinity_core:hnsw_handler;

import :hnsw_alg;
import :data_store;
import :vec_store_type;
import :dist_func_l2;
import :dist_func_ip;
import :hnsw_common;
import :index_hnsw;
import :infinity_exception;
import :index_base;
import :logger;
import :infinity_context;
import :logger;
import :base_memindex;
import :memindex_tracer;
import :buffer_handle;
import :config;
import :chunk_index_meta;

import std;
import third_party;

import internal_types;
import embedding_info;
import column_def;

namespace infinity {

class BufferManager;
struct ColumnVector;
class BufferObj;
class LocalFileHandle;

using AbstractHnsw = std::variant<std::unique_ptr<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqCosVecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqIPVecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqL2VecStoreType<float>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQCosVecStoreType<float, i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQIPVecStoreType<float, i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<LVQL2VecStoreType<float, i8, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqCosVecStoreType<float, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqIPVecStoreType<float, true>, SegmentOffset>>,
                                  std::unique_ptr<KnnHnsw<RabitqL2VecStoreType<float, true>, SegmentOffset>>,

                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<u8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<u8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<u8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqCosVecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqIPVecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqL2VecStoreType<float>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<float, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<float, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<float, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<u8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<u8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<u8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainCosVecStoreType<i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainIPVecStoreType<i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<PlainL2VecStoreType<i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQCosVecStoreType<float, i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQIPVecStoreType<float, i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<LVQL2VecStoreType<float, i8, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqCosVecStoreType<float, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqIPVecStoreType<float, true>, SegmentOffset, false>>,
                                  std::unique_ptr<KnnHnsw<RabitqL2VecStoreType<float, true>, SegmentOffset, false>>,
                                  std::nullptr_t>;

export struct HnswHandler {
public:
    HnswHandler() : hnsw_(nullptr) {}
    virtual ~HnswHandler() {}
    HnswHandler(const HnswHandler &) = delete;
    HnswHandler &operator=(const HnswHandler &) = delete;

    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem = true);

    HnswHandler(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem = true);

    static std::unique_ptr<HnswHandler> Make(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem = true);

    template <typename DistanceT, typename LabelT, typename Filter = std::nullopt_t, bool WithLock = true>
    std::tuple<size_t, std::unique_ptr<DistanceT[]>, std::unique_ptr<LabelT[]>>
    SearchIndex(const auto &q, size_t k, const Filter &filter, const KnnSearchOption &option = {}) const {
        std::tuple<size_t, std::unique_ptr<DistanceT[]>, std::unique_ptr<LabelT[]>> res{};
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
    std::tuple<size_t, std::unique_ptr<DistanceT[]>, std::unique_ptr<LabelT[]>>
    SearchIndex(const auto *q, size_t k, const KnnSearchOption &option = {}) const {
        std::tuple<size_t, std::unique_ptr<DistanceT[]>, std::unique_ptr<LabelT[]>> res{};
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
    static void InsertVecs(Index &index, Iter &&iter, const HnswInsertConfig &config, size_t &mem_usage, size_t kBuildBucketSize = 1024) {
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
                size_t mem1 = index->mem_usage();
                auto [start, end] = index->StoreData(std::forward<Iter>(iter), config);
                size_t bucket_size = std::max(kBuildBucketSize, size_t(end - start - 1) / thread_pool.size() + 1);
                size_t bucket_n = (end - start - 1) / bucket_size + 1;

                std::vector<std::future<void>> futs;
                futs.reserve(bucket_n);
                for (size_t i = 0; i < bucket_n; ++i) {
                    size_t i1 = start + i * bucket_size;
                    size_t i2 = std::min(i1 + bucket_size, size_t(end));
                    futs.emplace_back(thread_pool.push([&index, i1, i2](int id) {
                        for (size_t j = i1; j < i2; ++j) {
                            index->Build(j);
                        }
                    }));
                }
                for (auto &fut : futs) {
                    fut.get();
                }
                size_t mem2 = index->mem_usage();
                mem_usage = mem2 - mem1;
            }
        }
    }

public:
    size_t InsertVecs(SegmentOffset block_offset,
                      const ColumnVector &col,
                      BlockOffset offset,
                      BlockOffset row_count,
                      const HnswInsertConfig &config = kDefaultHnswInsertConfig,
                      size_t kBuildBucketSize = 1024);

    template <typename Iter>
    size_t InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig, size_t kBuildBucketSize = 1024) {
        size_t mem_usage{};
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
    std::pair<VertexType, VertexType>
    StoreData(const auto *data, size_t dim, size_t vec_num, const HnswInsertConfig &option = kDefaultHnswInsertConfig) {
        std::pair<VertexType, VertexType> res{};
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

public:
    // LSG setting
    template <typename Iter>
    size_t InsertSampleVecs(Iter iter, size_t sample_num = std::numeric_limits<size_t>::max()) {
        size_t insert_num = 0;
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    insert_num = index->InsertSampleVecs(std::move(iter), sample_num);
                }
            },
            hnsw_);
        return insert_num;
    }
    size_t InsertSampleVecs(size_t sample_num, SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count);

    template <typename Iter>
    void InsertLSAvg(Iter iter, size_t row_count) {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    index->InsertLSAvg(std::move(iter), row_count);
                }
            },
            hnsw_);
    }
    void InsertLSAvg(SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count);

    void SetLSGParam();

public:
    // get infomation from hnsw_ (hnsw_ create in compile)
    size_t MemUsage() const;
    size_t GetRowCount() const;
    size_t GetSizeInBytes() const;
    std::pair<size_t, size_t> GetInfo() const;
    void Check() const;

public:
    // hnsw_ data operator
    void SaveToPtr(LocalFileHandle &file_handle) const;
    void Load(LocalFileHandle &file_handle);
    void LoadFromPtr(LocalFileHandle &file_handle, size_t file_size);
    void LoadFromPtr(const char *ptr, size_t size);
    void Build(VertexType vertex_i);
    void Optimize();
    void CompressToLVQ();
    void CompressToRabitq();

private:
    AbstractHnsw hnsw_ = nullptr;
};

export using HnswHandlerPtr = HnswHandler *;

export struct HnswIndexInMem : public BaseMemIndex {
public:
    HnswIndexInMem() : hnsw_handler_(nullptr) {}
    HnswIndexInMem(RowID begin_row_id, const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def)
        : begin_row_id_(begin_row_id), hnsw_handler_(HnswHandler::Make(index_base, column_def).release()), own_memory_(true) {}
    HnswIndexInMem(const HnswIndexInMem &) = delete;
    HnswIndexInMem &operator=(const HnswIndexInMem &) = delete;
    virtual ~HnswIndexInMem();

public:
    static std::unique_ptr<HnswIndexInMem> Make(RowID begin_row_id, const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def);

    static std::unique_ptr<HnswIndexInMem> Make(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def);

public:
    void InsertVecs(SegmentOffset block_offset,
                    const ColumnVector &col,
                    BlockOffset offset,
                    BlockOffset row_count,
                    const HnswInsertConfig &config = kDefaultHnswInsertConfig);

    template <typename Iter>
    void InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        size_t mem_usage = hnsw_handler_->InsertVecs(std::move(iter), config, kBuildBucketSize);
        row_count_ += iter.GetRowCount();
        IncreaseMemoryUsageBase(mem_usage);
    }

    void Dump(BufferObj *buffer_obj, size_t *dump_size_ptr = nullptr);

public:
    // LSG setting
    template <typename Iter>
    size_t InsertSampleVecs(Iter iter, size_t sample_num = std::numeric_limits<size_t>::max()) {
        return hnsw_handler_->InsertSampleVecs(std::move(iter), sample_num);
    }
    size_t InsertSampleVecs(size_t sample_num, SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count);
    template <typename Iter>

    void InsertLSAvg(Iter iter, size_t row_count) {
        hnsw_handler_->InsertLSAvg(std::move(iter), row_count);
    }
    void InsertLSAvg(SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count);

    void SetLSGParam();

public:
    RowID GetBeginRowID() const override { return begin_row_id_; }
    const HnswHandlerPtr &get() const { return hnsw_handler_; }
    HnswHandlerPtr *get_ptr() { return &hnsw_handler_; }
    size_t GetRowCount() const;
    size_t GetSizeInBytes() const;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

protected:
    MemIndexTracerInfo GetInfo() const override;

private:
    static constexpr size_t kBuildBucketSize = 1024;

    RowID begin_row_id_ = {};
    size_t row_count_ = 0;
    HnswHandlerPtr hnsw_handler_;
    bool own_memory_{};
    BufferHandle chunk_handle_{};
};

} // namespace infinity