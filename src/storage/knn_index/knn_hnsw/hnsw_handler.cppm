//
// Created by infiniflow on 25-5-13.
//

module;

#include <future>

namespace infinity {
struct SegmentEntry;
}

export module hnsw_handler;

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
import table_index_entry;
import buffer_handle;
import third_party;
import local_file_handle;
import default_values;
import block_column_iter;
import logical_type;
import multivector_util;
import segment_iter;
import column_vector;
import segment_index_entry;

namespace infinity {

class BufferManager;
struct ChunkIndexEntry;
struct BlockColumnEntry;
class BufferObj;

using AbstractHnsw = std::variant<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset> *,
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

template <typename DataType, bool OwnMem>
AbstractHnsw InitAbstractIndexT(const IndexHnsw *index_hnsw) {
    switch (index_hnsw->encode_type_) {
        case HnswEncodeType::kPlain: {
            if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else if (index_hnsw->build_type_ != HnswBuildType::kPlain) {
                return nullptr;
            }
            switch (index_hnsw->metric_type_) {
                case MetricType::kMetricL2: {
                    using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType>, SegmentOffset, OwnMem>;
                    return static_cast<HnswIndex *>(nullptr);
                }
                case MetricType::kMetricInnerProduct: {
                    using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType>, SegmentOffset, OwnMem>;
                    return static_cast<HnswIndex *>(nullptr);
                }
                case MetricType::kMetricCosine: {
                    using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType>, SegmentOffset, OwnMem>;
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
            } else if (index_hnsw->build_type_ == HnswBuildType::kPlain) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<LVQL2VecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return static_cast<HnswIndex *>(nullptr);
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
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

template <bool OwnMem>
AbstractHnsw InitAbstractIndexT(const IndexBase *index_base, const ColumnDef *column_def) {
    const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());

    switch (embedding_info->Type()) {
        case EmbeddingDataType::kElemFloat: {
            return InitAbstractIndexT<float, OwnMem>(index_hnsw);
        }
        case EmbeddingDataType::kElemUInt8: {
            return InitAbstractIndexT<u8, OwnMem>(index_hnsw);
        }
        case EmbeddingDataType::kElemInt8: {
            return InitAbstractIndexT<i8, OwnMem>(index_hnsw);
        }
        default: {
            return nullptr;
        }
    }
}

export class HnswHandler {
public:
    HnswHandler() : hnsw_(nullptr) {}
    HnswHandler(const void *ptr) : hnsw_(*reinterpret_cast<const AbstractHnsw *>(ptr)) {}
    ~HnswHandler() {
        std::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    delete arg;
                }
            },
            hnsw_);
    }

    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true) {
        if (own_mem) {
            return InitAbstractIndexT<true>(index_base, column_def);
        } else {
            return InitAbstractIndexT<false>(index_base, column_def);
        }
    }

    HnswHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true)
        : hnsw_(InitAbstractIndex(index_base, column_def, own_mem)) {
        const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
        const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());

        SizeT chunk_size = index_hnsw->block_size_;
        SizeT max_chunk_num = (DEFAULT_SEGMENT_CAPACITY - 1) / chunk_size + 1;

        SizeT dim = embedding_info->Dimension();
        SizeT M = index_hnsw->M_;
        SizeT ef_construction = index_hnsw->ef_construction_;
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (IndexT::kOwnMem) {
                        index = IndexT::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    } else {
                        UnrecoverableError("HnswHandler::HnswHandler: index does not own memory");
                    }
                }
            },
            hnsw_);
    }

    static UniquePtr<HnswHandler> Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem = true) {
        return MakeUnique<HnswHandler>(index_base, column_def, own_mem);
    }

    //     template <typename DistanceT, typename LabelT, typename Filter = NoneType, bool WithLock = true>
    //     Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>>
    //     KnnSearch(const auto &q, SizeT k, const Filter &filter, const KnnSearchOption &option = {}) const {
    //         Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>> res{};
    //         std::visit(
    //             [&](auto &&arg) {
    //                 using T = std::decay_t<decltype(arg)>;
    //                 if constexpr (!std::is_same_v<T, std::nullptr_t>) {
    //                     res = arg->template KnnSearch<Filter, WithLock>(q, k, filter, option);
    //                 }
    //             },
    //             hnsw_);
    //         return res;
    //     }

    template <typename DistanceT, typename LabelT, bool WithLock = true>
    Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>>
    SearchIndex(const auto* q, SizeT k, const KnnSearchOption &option = {}) const {
        Tuple<SizeT, UniquePtr<DistanceT[]>, UniquePtr<LabelT[]>> res{};
        std::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    using IndexT = std::decay_t<decltype(*arg)>;
                    res = arg->template KnnSearch<WithLock>(reinterpret_cast<typename IndexT::QueryVecType>(q), k, option);
                }
            },
            hnsw_);
        return res;
    }

    private:
        template <typename Iter, typename Index>
        static void InsertVecs(Index &index,
                                Iter &&iter,
                                const HnswInsertConfig &config,
                                SizeT &mem_usage,
                                SizeT kBuildBucketSize) {
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
    // SizeT InsertVecs(SizeT block_offset,
    //                 BlockColumnEntry *block_column_entry,
    //                 BufferManager *buffer_manager,
    //                 SizeT row_offset,
    //                 SizeT row_count,
    //                 SizeT kBuildBucketSize,
    //                 const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
    //     SizeT mem_usage{};
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (!std::is_same_v<T, std::nullptr_t>) {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     using DataType = typename IndexT::DataType;
    //
    //                     switch (const auto &column_data_type = block_column_entry->column_type(); column_data_type->type()) {
    //                         case LogicalType::kEmbedding: {
    //                             MemIndexInserterIter<DataType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
    //                             HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             break;
    //                         }
    //                         case LogicalType::kMultiVector: {
    //                             MemIndexInserterIter<MultiVectorRef<DataType>> iter(block_offset,
    //                                                                                 block_column_entry,
    //                                                                                 buffer_manager,
    //                                                                                 row_offset,
    //                                                                                 row_count);
    //                             HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             break;
    //                         }
    //                         default: {
    //                             UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
    //                             break;
    //                         }
    //                     }
    //                 } else {
    //                     UnrecoverableError("HnswHandler::InsertVecs: index does not own memory");
    //                 }
    //             }
    //         },
    //         hnsw_);
    //     return mem_usage;
    // }

    // SizeT InsertVecs(SegmentOffset block_offset,
    //                 const ColumnVector &col,
    //                 BlockOffset offset,
    //                 BlockOffset row_count,
    //                 SizeT kBuildBucketSize,
    //                 const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
    //     SizeT mem_usage{};
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (!std::is_same_v<T, std::nullptr_t>) {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     using DataType = typename IndexT::DataType;
    //                     switch (const auto &column_data_type = col.data_type(); column_data_type->type()) {
    //                         case LogicalType::kEmbedding: {
    //                             // MemIndexInserterIter<DataType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
    //                             MemIndexInserterIter1<DataType> iter(block_offset, col, offset, row_count);
    //                             HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             break;
    //                         }
    //                         case LogicalType::kMultiVector: {
    //                             MemIndexInserterIter1<MultiVectorRef<DataType>> iter(block_offset, col, offset, row_count);
    //                             // MemIndexInserterIter<MultiVectorRef<DataType>> iter(block_offset,
    //                             //                                                     block_column_entry,
    //                             //                                                     buffer_manager,
    //                             //                                                     row_offset,
    //                             //                                                     row_count);
    //                             HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             break;
    //                         }
    //                         default: {
    //                             UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
    //                             break;
    //                         }
    //                     }
    //                 } else {
    //                     UnrecoverableError("HnswHandler::InsertVecs: index does not own memory");
    //                 }
    //             }
    //         },
    //         hnsw_);
    //    return mem_usage;
    // }

    // SizeT InsertVecs(const SegmentEntry *segment_entry,
    //                 BufferManager *buffer_mgr,
    //                 SizeT column_id,
    //                 TxnTimeStamp begin_ts,
    //                 bool check_ts,
    //                 SizeT kBuildBucketSize,
    //                 const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
    //     SizeT mem_usage{};
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (!std::is_same_v<T, std::nullptr_t>) {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (!IndexT::kOwnMem) {
    //                     UnrecoverableError("HnswHandler::InsertVecs: index does not own memory");
    //                 } else {
    //                     using DataType = typename IndexT::DataType;
    //
    //                     switch (const auto &column_data_type = segment_entry->GetTableEntry()->GetColumnDefByID(column_id)->type();
    //                             column_data_type->type()) {
    //                         case LogicalType::kEmbedding: {
    //                             if (check_ts) {
    //                                 OneColumnIterator<DataType> iter(segment_entry, buffer_mgr, column_id, begin_ts);
    //                                 HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             } else {
    //                                 OneColumnIterator<DataType, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
    //                                 HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             }
    //                             break;
    //                         }
    //                         case LogicalType::kMultiVector: {
    //                             const auto ele_size = column_data_type->type_info()->Size();
    //                             if (check_ts) {
    //                                 OneColumnIterator<MultiVectorRef<DataType>> iter(segment_entry, buffer_mgr, column_id, begin_ts, ele_size);
    //                                 HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             } else {
    //                                 OneColumnIterator<MultiVectorRef<DataType>, false> iter(segment_entry, buffer_mgr, column_id, begin_ts,
    //                                 ele_size); HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
    //                             }
    //                             break;
    //                         }
    //                         default: {
    //                             UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
    //                             break;
    //                         }
    //                     }
    //                 }
    //             }
    //         },
    //         hnsw_);
    //     return mem_usage;
    // }

    // void InsertVecs(const SegmentEntry *segment_entry,
    //                 BufferManager *buffer_mgr,
    //                 SizeT column_id,
    //                 TxnTimeStamp begin_ts,
    //                 u32 row_count,
    //                 const HnswInsertConfig &config = kDefaultHnswInsertConfig) const {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (!std::is_same_v<T, std::nullptr_t>) {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     using DataType = typename IndexT::DataType;
    //                     CappedOneColumnIterator<DataType, true> iter(segment_entry,
    //                                                                       buffer_mgr,
    //                                                                       column_id,
    //                                                                       begin_ts,
    //                                                                       row_count);
    //                     index->InsertVecs(std::move(iter), config);
    //                 } else {
    //                     UnrecoverableError("Invalid index type.");
    //                 }
    //             }
    //         },
    //         hnsw_);
    // }

    template <typename Iter>
    SizeT InsertVecs(Iter iter, SizeT kBuildBucketSize, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
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

    // get infomation from hnsw_ (hnsw_ create in compile)
    SizeT MemUsage() const {
        return std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return SizeT(0);
                } else {
                    return index->mem_usage();
                }
            },
            hnsw_);
    }
    SizeT GetRowCount() const {
        return std::visit(
            [](auto &&index) {
                using IndexType = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                    return SizeT(0);
                } else {
                    return index->GetVecNum();
                }
            },
            hnsw_);
    }
    SizeT GetSizeInBytes() const {
        return std::visit(
            [](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return SizeT(0);
                } else {
                    using IndexT = typename std::remove_pointer_t<T>;
                    if constexpr (IndexT::kOwnMem) {
                        return index->GetSizeInBytes();
                    } else {
                        return SizeT(0);
                    }
                }
            },
            hnsw_);
    }
    Pair<SizeT, SizeT> GetInfo() const {
        return {MemUsage(), GetRowCount()};
    }
    void GetType() const {
        return std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    std::cout << typeid(T).name() << std::endl;
                }
            },
            hnsw_);
    }
    template <typename Iterator>
    Pair<VertexType, VertexType> StoreData(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        return std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return Pair<VertexType, VertexType>{};
                } else {
                    return index->StoreData(std::move(iter));
                }
            },
            hnsw_);
    }
    template<typename LabelT>
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

    // hnsw_ data operator
    void Check() const {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    index->Check();
                }
            },
            hnsw_);
    }
    void SetLSGParam(float alpha, UniquePtr<float[]> avg) {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (IndexT::LSG) {
                        index->distance().SetLSGParam(alpha, std::move(avg));
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
    }
    void SaveToPtr(LocalFileHandle &file_handle) const {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (IndexT::kOwnMem) {
                        index->SaveToPtr(file_handle);
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
    }
    // void Load(LocalFileHandle &file_handle) const {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                 UnrecoverableError("Invalid index type.");
    //             } else {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     index = IndexT::Load(file_handle).release();
    //                 } else {
    //                     UnrecoverableError("Invalid index type.");
    //                 }
    //             }
    //         },
    //         hnsw_);
    //
    // }
    void LoadFromPtr(LocalFileHandle &file_handle, SizeT file_size) {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    if constexpr (IndexT::kOwnMem) {
                        index = IndexT::LoadFromPtr(file_handle, file_size).release();
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
    }
    // void LoadFromPtr(const char *ptr, SizeT size) const {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                 UnrecoverableError("Invalid index type.");
    //             } else {
    //                 using IndexT = std::decay_t<decltype(*index)>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     UnrecoverableError("Invalid index type.");
    //                 } else {
    //                     index = IndexT::LoadFromPtr(ptr, size).release();
    //                 }
    //             }
    //         },
    //         hnsw_);
    // }
    // void Build(VertexType vertex_i) {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                 UnrecoverableError("Invalid index type.");
    //             } else {
    //                 index->Build(vertex_i);
    //             }
    //         },
    //         hnsw_);
    // }
    // void Optimize() {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                 UnrecoverableError("Invalid index type.");
    //             } else {
    //                 index->Optimize();
    //             }
    //         },
    //         hnsw_);
    // }
    void CompressToLVQ() {
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    using IndexT = typename std::remove_pointer_t<T>;
                    if constexpr (IndexT::kOwnMem) {
                        using HnswIndexDataType = typename std::remove_pointer_t<T>::DataType;
                        if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
                            UnrecoverableError("Invalid index type.");
                        } else {
                            auto *p = std::move(*index).CompressToLVQ().release();
                            delete index;
                            hnsw_ = p;
                        }
                    } else {
                        UnrecoverableError("Invalid index type.");
                    }
                }
            },
            hnsw_);
    }
    // void Optimize(bool compress_to_lvq, bool lvq_avg) {
    //     std::visit(
    //         [&](auto &&index) {
    //             using T = std::decay_t<decltype(index)>;
    //             if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                 UnrecoverableError("Invalid index type.");
    //             } else {
    //                 using IndexT = typename std::remove_pointer_t<T>;
    //                 if constexpr (IndexT::kOwnMem) {
    //                     using HnswIndexDataType = typename std::remove_pointer_t<T>::DataType;
    //                     if (compress_to_lvq) {
    //                         if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
    //                             UnrecoverableError("Invalid index type.");
    //                         } else {
    //                             auto *p = std::move(*index).CompressToLVQ().release();
    //                             delete index;
    //                             hnsw_ = p;
    //                         }
    //                     }
    //                     if (lvq_avg) {
    //                         index->Optimize();
    //                     }
    //                 } else {
    //                     UnrecoverableError("Invalid index type.");
    //                 }
    //             }
    //         },
    //         hnsw_);
    // }

private:
    AbstractHnsw hnsw_ = nullptr;
};

export struct HnswIndexInMem : public BaseMemIndex {
public:
     HnswIndexInMem() : hnsw_handler_(nullptr) {}
     HnswIndexInMem(RowID begin_row_id,
                     const IndexBase *index_base,
                     const ColumnDef *column_def,
                     SegmentIndexEntry *segment_index_entry,
                     bool trace)
         : begin_row_id_(begin_row_id), hnsw_handler_(HnswHandler::Make(index_base, column_def).release()),
             segment_index_entry_(segment_index_entry), trace_(trace), own_memory_(true) {}
     HnswIndexInMem(const HnswIndexInMem &) = delete;
     HnswIndexInMem &operator=(const HnswIndexInMem &) = delete;
     virtual ~HnswIndexInMem() {
         if (hnsw_handler_ != nullptr) {
             delete hnsw_handler_;
         }
         if (trace_) {
             auto *storage = InfinityContext::instance().storage();
             if (storage == nullptr) {
                 return;
             }
             auto *memindex_tracer = storage->memindex_tracer();
             if (memindex_tracer != nullptr) {
                 memindex_tracer->DecreaseMemUsed(0);
             }
         }
     }

//     static UniquePtr<HnswIndexInMem> Make(RowID begin_row_id,
//                                             const IndexBase *index_base,
//                                             const ColumnDef *column_def,
//                                             SegmentIndexEntry *segment_index_entry,
//                                             bool trace = false) {
//         auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, segment_index_entry, trace);
//         if (trace) {
//             auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
//             memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->mem_usage());
//         }
//         return memidx;
//     }

     static UniquePtr<HnswIndexInMem> Make(const IndexBase *index_base,
                                             const ColumnDef *column_def,
                                             bool trace = false) {
         RowID begin_row_id{0, 0};
         auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, nullptr, trace);
         if (trace) {
             auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
             memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->MemUsage());
         }
         return memidx;
     }

//     void InsertVecs(SizeT block_offset,
//                     BlockColumnEntry *block_column_entry,
//                     BufferManager *buffer_manager,
//                     SizeT row_offset,
//                     SizeT row_count,
//                     const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
//         SizeT mem_usage = hnsw_handler_->InsertVecs(block_offset, block_column_entry, buffer_manager, row_offset, row_count, kBuildBucketSize,
//         config); this->IncreaseMemoryUsageBase(mem_usage);
//     }
//
//     void InsertVecs(SegmentOffset block_offset,
//                     const ColumnVector &col,
//                     BlockOffset offset,
//                     BlockOffset row_count,
//                     const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
//         SizeT mem_usage = hnsw_handler_->InsertVecs(block_offset, col, offset, row_count, kBuildBucketSize, config);
//         this->IncreaseMemoryUsageBase(mem_usage);
//     }
//
//     void InsertVecs(const SegmentEntry *segment_entry,
//                     BufferManager *buffer_mgr,
//                     SizeT column_id,
//                     TxnTimeStamp begin_ts,
//                     bool check_ts,
//                     const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
//         SizeT mem_usage = hnsw_handler_->InsertVecs(segment_entry, buffer_mgr, column_id, begin_ts, check_ts, kBuildBucketSize, config);
//         this->IncreaseMemoryUsageBase(mem_usage);
//     }
//
     template <typename Iter>
     void InsertVecs(Iter iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig, bool trace = true) {
         SizeT mem_usage = hnsw_handler_->InsertVecs(std::move(iter), kBuildBucketSize, config);
         if (trace) {
             IncreaseMemoryUsageBase(mem_usage);
         }
     }

     // get/set
     RowID GetBeginRowID() const { return begin_row_id_; }
     const HnswHandler &get() const { return *hnsw_handler_; }
     HnswHandler *get_ptr() { return hnsw_handler_; }
     void SetLSGParam(float alpha, UniquePtr<float[]> avg) {
         hnsw_handler_->SetLSGParam(alpha, std::move(avg));
     }
//     SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr, SizeT *dump_size_ptr = nullptr) {
//         SizeT row_count = 0;
//         SizeT index_size = 0;
//         SizeT dump_size = 0;
//         trace_ = false;
//         hnsw_handler_->Dump(row_count, index_size, dump_size);
//         auto new_chunk_indey_entry = segment_index_entry->CreateHnswIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);
//         if (dump_size_ptr != nullptr) {
//             *dump_size_ptr = dump_size;
//         }
//
//         BufferHandle handle = new_chunk_indey_entry->GetIndex();
//         auto *data_ptr = static_cast<HnswHandler *>(handle.GetDataMut());
//         *data_ptr = hnsw_handler_;
//         own_memory_ = false;
//         chunk_handle_ = std::move(handle);
//         return new_chunk_indey_entry;
//     }
//     void Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr = nullptr) {
//         trace_ = false;
//         if (dump_size_ptr != nullptr) {
//             SizeT dump_size = 0;
//             hnsw_handler_->Dump(dump_size);
//             *dump_size_ptr = dump_size;
//         }
//
//         BufferHandle handle = buffer_obj->Load();
//         auto *data_ptr = static_cast<HnswHandler *>(handle.GetDataMut());
//         *data_ptr = hnsw_handler_;
//         own_memory_ = false;
//         chunk_handle_ = std::move(handle);
//     }
//     SizeT GetRowCount() const {
//         return hnsw_handler_->GetRowCount();
//     }
//     SizeT GetSizeInBytes() const {
//         return hnsw_handler_->GetSizeInBytes();
//     }
//     void SetSegmentEntry(SegmentIndexEntry *segment_index_entry) {
//         segment_index_entry_ = segment_index_entry;
//         begin_row_id_ = RowID(segment_index_entry->segment_id(), 0);
//     }
     TableIndexEntry *table_index_entry() const override {
         return segment_index_entry_->table_index_entry();
     }

protected:
    MemIndexTracerInfo GetInfo() const override {
        auto [mem_used, row_cnt] = hnsw_handler_->GetInfo();

        if (segment_index_entry_ == nullptr) {
            return MemIndexTracerInfo(MakeShared<String>(index_name_), MakeShared<String>(table_name_), MakeShared<String>(db_name_), mem_used,
            row_cnt);
        }

        auto *table_index_entry = segment_index_entry_->table_index_entry();
        SharedPtr<String> index_name = table_index_entry->GetIndexName();
        auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
        SharedPtr<String> table_name = table_entry->GetTableName();
        SharedPtr<String> db_name = table_entry->GetDBName();
        return MemIndexTracerInfo(index_name, table_name, db_name, mem_used, row_cnt);
    }

private:
    static constexpr SizeT kBuildBucketSize = 1024;

    RowID begin_row_id_ = {};
    HnswHandler *hnsw_handler_;

    SegmentIndexEntry *segment_index_entry_{};
    bool trace_{};
    bool own_memory_{};
    BufferHandle chunk_handle_{};
};

} // namespace infinity