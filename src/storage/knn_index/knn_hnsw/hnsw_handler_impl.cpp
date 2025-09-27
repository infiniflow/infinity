//
// Created by infiniflow on 25-5-13.
//

module;

module infinity_core:hnsw_handler.impl;

import :hnsw_handler;
import :buffer_manager;
import :buffer_handle;
import :block_column_iter;
import :memindex_tracer;
import :default_values;
import :multivector_util;
import :infinity_exception;
import :column_vector;
import :local_file_handle;
import :chunk_index_meta;
import :plain_vec_store;

import third_party;

import embedding_info;
import embedding_type;
import column_def;
import row_id;
import logical_type;

namespace infinity {

template <typename DataType, bool OwnMem>
AbstractHnsw InitAbstractIndexT(const IndexHnsw *index_hnsw) {
    switch (index_hnsw->encode_type_) {
        case HnswEncodeType::kPlain: {
            if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else if (index_hnsw->build_type_ == HnswBuildType::kPlain) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else {
                return nullptr;
            }
        }
        case HnswEncodeType::kLVQ: {
            if constexpr (std::is_same_v<DataType, u8> || std::is_same_v<DataType, i8>) {
                return nullptr;
            } else if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<LVQL2VecStoreType<DataType, i8, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else if (index_hnsw->build_type_ == HnswBuildType::kPlain) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<LVQL2VecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else {
                return nullptr;
            }
        }
        case HnswEncodeType::kRabitq: {
            if constexpr (!std::is_same_v<DataType, f32>) {
                return nullptr;
            } else if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<RabitqL2VecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<RabitqIPVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<RabitqCosVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else if (index_hnsw->build_type_ == HnswBuildType::kPlain) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<RabitqL2VecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<RabitqIPVecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<RabitqCosVecStoreType<DataType>, SegmentOffset, OwnMem>;
                        return std::unique_ptr<HnswIndex>();
                    }
                    default: {
                        return nullptr;
                    }
                }
            } else {
                return nullptr;
            }
        }
        default: {
            return nullptr;
        }
    }
}

template <bool OwnMem>
AbstractHnsw InitAbstractIndexT(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def) {
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

AbstractHnsw HnswHandler::InitAbstractIndex(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem) {
    if (own_mem) {
        return InitAbstractIndexT<true>(index_base, column_def);
    } else {
        return InitAbstractIndexT<false>(index_base, column_def);
    }
}

HnswHandler::HnswHandler(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem)
    : hnsw_(InitAbstractIndex(index_base, column_def, own_mem)) {
    if (!own_mem)
        return;
    const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());

    size_t chunk_size = index_hnsw->block_size_;
    size_t max_chunk_num = (DEFAULT_SEGMENT_CAPACITY - 1) / chunk_size + 1;

    size_t dim = embedding_info->Dimension();
    size_t M = index_hnsw->M_;
    size_t ef_construction = index_hnsw->ef_construction_;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = IndexT::Make(chunk_size, max_chunk_num, dim, M, ef_construction);
                    if constexpr (IndexT::LSG) {
                        index->InitLSGBuilder(index_hnsw, column_def);
                    }
                } else {
                    UnrecoverableError("HnswHandler::HnswHandler: index does not own memory");
                }
            }
        },
        hnsw_);
}

std::unique_ptr<HnswHandler> HnswHandler::Make(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def, bool own_mem) {
    return std::make_unique<HnswHandler>(index_base, column_def, own_mem);
}

size_t HnswHandler::InsertVecs(SegmentOffset block_offset,
                               const ColumnVector &col,
                               BlockOffset offset,
                               BlockOffset row_count,
                               const HnswInsertConfig &config,
                               size_t kBuildBucketSize) {
    size_t mem_usage{};
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using DataType = typename IndexT::DataType;
                    switch (const auto &column_data_type = col.data_type(); column_data_type->type()) {
                        case LogicalType::kEmbedding: {
                            MemIndexInserterIter1<DataType> iter(block_offset, col, offset, row_count);
                            HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
                            break;
                        }
                        case LogicalType::kMultiVector: {
                            MemIndexInserterIter1<MultiVectorRef<DataType>> iter(block_offset, col, offset, row_count);
                            HnswHandler::InsertVecs(index, std::move(iter), config, mem_usage, kBuildBucketSize);
                            break;
                        }
                        default: {
                            UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                            break;
                        }
                    }
                } else {
                    UnrecoverableError("HnswHandler::InsertVecs: index does not own memory");
                }
            }
        },
        hnsw_);
    return mem_usage;
}

size_t
HnswHandler::InsertSampleVecs(size_t sample_num, SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count) {
    size_t insert_num = 0;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                using DataType = typename IndexT::DataType;
                switch (const auto &column_data_type = col.data_type(); column_data_type->type()) {
                    case LogicalType::kEmbedding: {
                        MemIndexInserterIter1<DataType> iter(block_offset, col, offset, row_count);
                        insert_num = index->InsertSampleVecs(iter, sample_num);
                        break;
                    }
                    case LogicalType::kMultiVector: {
                        MemIndexInserterIter1<MultiVectorRef<DataType>> iter(block_offset, col, offset, row_count);
                        insert_num = index->InsertSampleVecs(iter, sample_num);
                        break;
                    }
                    default: {
                        UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                        break;
                    }
                }
            }
        },
        hnsw_);
    return insert_num;
}

void HnswHandler::InsertLSAvg(SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                using DataType = typename IndexT::DataType;
                switch (const auto &column_data_type = col.data_type(); column_data_type->type()) {
                    case LogicalType::kEmbedding: {
                        MemIndexInserterIter1<DataType> iter(block_offset, col, offset, row_count);
                        index->InsertLSAvg(iter, row_count);
                        break;
                    }
                    case LogicalType::kMultiVector: {
                        MemIndexInserterIter1<MultiVectorRef<DataType>> iter(block_offset, col, offset, row_count);
                        index->InsertLSAvg(iter, row_count);
                        break;
                    }
                    default: {
                        UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                        break;
                    }
                }
            }
        },
        hnsw_);
}

void HnswHandler::SetLSGParam() {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::LSG) {
                    index->SetLSGParam();
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

size_t HnswHandler::MemUsage() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return size_t(0);
            } else {
                return index->mem_usage();
            }
        },
        hnsw_);
}

size_t HnswHandler::GetRowCount() const {
    return std::visit(
        [](auto &&index) {
            using IndexType = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                return size_t(0);
            } else {
                return index->GetVecNum();
            }
        },
        hnsw_);
}

size_t HnswHandler::GetSizeInBytes() const {
    return std::visit(
        [](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return size_t(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->GetSizeInBytes();
                } else {
                    return size_t(0);
                }
            }
        },
        hnsw_);
}

std::pair<size_t, size_t> HnswHandler::GetInfo() const { return {MemUsage(), GetRowCount()}; }

void HnswHandler::Check() const {
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

void HnswHandler::SaveToPtr(LocalFileHandle &file_handle) const {
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

void HnswHandler::Load(LocalFileHandle &file_handle) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = IndexT::Load(file_handle);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

void HnswHandler::LoadFromPtr(LocalFileHandle &file_handle, size_t file_size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = IndexT::LoadFromPtr(file_handle, file_size);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

void HnswHandler::LoadFromPtr(const char *ptr, size_t size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    UnrecoverableError("Invalid index type.");
                } else {
                    index = IndexT::LoadFromPtr(ptr, size);
                }
            }
        },
        hnsw_);
}

void HnswHandler::Build(VertexType vertex_i) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->Build(vertex_i);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

void HnswHandler::Optimize() {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->Optimize();
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

void HnswHandler::CompressToLVQ() {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using HnswIndexDataType = IndexT::DataType;
                    if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
                        UnrecoverableError("Invalid index type.");
                    } else {
                        hnsw_ = std::move(*index).CompressToLVQ();
                    }
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

void HnswHandler::CompressToRabitq() {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using HnswIndexDataType = IndexT::DataType;
                    if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
                        UnrecoverableError("Invalid index type.");
                    } else {
                        hnsw_ = std::move(*index).CompressToRabitq();
                    }
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        hnsw_);
}

HnswIndexInMem::~HnswIndexInMem() {
    size_t mem_usage = hnsw_handler_->MemUsage();
    if (own_memory_ && hnsw_handler_ != nullptr) {
        delete hnsw_handler_;
    }

    auto *storage = InfinityContext::instance().storage();
    if (storage == nullptr) {
        return;
    }
    auto *memindex_tracer = storage->memindex_tracer();
    if (memindex_tracer != nullptr) {
        memindex_tracer->DecreaseMemUsed(mem_usage);
    }
}

std::unique_ptr<HnswIndexInMem> HnswIndexInMem::Make(RowID begin_row_id, const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def) {
    auto memidx = std::make_unique<HnswIndexInMem>(begin_row_id, index_base, column_def);

    auto *storage = InfinityContext::instance().storage();
    if (storage != nullptr) {
        auto *memindex_tracer = storage->memindex_tracer();
        if (memindex_tracer != nullptr) {
            memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->MemUsage());
        }
    }
    return memidx;
}

std::unique_ptr<HnswIndexInMem> HnswIndexInMem::Make(const IndexBase *index_base, std::shared_ptr<ColumnDef> column_def) {
    RowID begin_row_id{0, 0};
    auto memidx = std::make_unique<HnswIndexInMem>(begin_row_id, index_base, column_def);

    auto *storage = InfinityContext::instance().storage();
    if (storage != nullptr) {
        auto *memindex_tracer = storage->memindex_tracer();
        if (memindex_tracer != nullptr) {
            memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->MemUsage());
        }
    }
    return memidx;
}

MemIndexTracerInfo HnswIndexInMem::GetInfo() const {
    auto [mem_used, row_cnt] = hnsw_handler_->GetInfo();
    return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                              std::make_shared<std::string>(table_name_),
                              std::make_shared<std::string>(db_name_),
                              mem_used,
                              row_cnt);
}

void HnswIndexInMem::InsertVecs(SegmentOffset block_offset,
                                const ColumnVector &col,
                                BlockOffset offset,
                                BlockOffset row_count,
                                const HnswInsertConfig &config) {
    size_t mem_usage = hnsw_handler_->InsertVecs(block_offset, col, offset, row_count, config, kBuildBucketSize);
    row_count_ += row_count;
    IncreaseMemoryUsageBase(mem_usage);
}

void HnswIndexInMem::Dump(BufferObj *buffer_obj, size_t *dump_size_ptr) {
    if (dump_size_ptr != nullptr) {
        size_t dump_size = hnsw_handler_->MemUsage();
        *dump_size_ptr = dump_size;
    }

    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<HnswHandlerPtr *>(handle.GetDataMut());
    *data_ptr = hnsw_handler_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
}

size_t
HnswIndexInMem::InsertSampleVecs(size_t sample_num, SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count) {
    return hnsw_handler_->InsertSampleVecs(sample_num, block_offset, offset, col, row_count);
}

void HnswIndexInMem::InsertLSAvg(SegmentOffset block_offset, BlockOffset offset, const ColumnVector &col, BlockOffset row_count) {
    hnsw_handler_->InsertLSAvg(block_offset, offset, col, row_count);
}

void HnswIndexInMem::SetLSGParam() { hnsw_handler_->SetLSGParam(); }

size_t HnswIndexInMem::GetRowCount() const { return row_count_; }

size_t HnswIndexInMem::GetSizeInBytes() const { return hnsw_handler_->GetSizeInBytes(); }

const ChunkIndexMetaInfo HnswIndexInMem::GetChunkIndexMetaInfo() const {
    return ChunkIndexMetaInfo{"", begin_row_id_, GetRowCount(), 0, GetSizeInBytes()};
}

} // namespace infinity