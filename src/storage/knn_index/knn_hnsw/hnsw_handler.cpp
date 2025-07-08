//
// Created by infiniflow on 25-5-13.
//

module;

module infinity_core;

import :buffer_manager;
import :buffer_handle;
import :block_column_iter;
import :memindex_tracer;
import :default_values;
import logical_type;
import :multivector_util;
import :infinity_exception;
import :third_party;
import :column_vector;
import :local_file_handle;
import :chunk_index_meta;
import embedding_info;
import embedding_type;
import column_def;
import row_id;

namespace infinity {

template <typename DataType, bool OwnMem>
AbstractHnsw InitAbstractIndexT(const IndexHnsw *index_hnsw) {
    switch (index_hnsw->encode_type_) {
        case HnswEncodeType::kPlain: {
            if (index_hnsw->build_type_ == HnswBuildType::kLSG) {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return UniquePtr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return UniquePtr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType, true>, SegmentOffset, OwnMem>;
                        return UniquePtr<HnswIndex>();
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
                    return UniquePtr<HnswIndex>();
                }
                case MetricType::kMetricInnerProduct: {
                    using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType>, SegmentOffset, OwnMem>;
                    return UniquePtr<HnswIndex>();
                }
                case MetricType::kMetricCosine: {
                    using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType>, SegmentOffset, OwnMem>;
                    return UniquePtr<HnswIndex>();
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
                        return UniquePtr<HnswIndex>();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return UniquePtr<HnswIndex>();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8>, SegmentOffset, OwnMem>;
                        return UniquePtr<HnswIndex>();
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

AbstractHnsw HnswHandler::InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    if (own_mem) {
        return InitAbstractIndexT<true>(index_base, column_def);
    } else {
        return InitAbstractIndexT<false>(index_base, column_def);
    }
}

HnswHandler::HnswHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem)
    : hnsw_(InitAbstractIndex(index_base, column_def, own_mem)) {
    if (!own_mem)
        return;
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
                    index = IndexT::Make(chunk_size, max_chunk_num, dim, M, ef_construction);
                } else {
                    UnrecoverableError("HnswHandler::HnswHandler: index does not own memory");
                }
            }
        },
        hnsw_);
}

UniquePtr<HnswHandler> HnswHandler::Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    return MakeUnique<HnswHandler>(index_base, column_def, own_mem);
}

SizeT HnswHandler::InsertVecs(SegmentOffset block_offset,
                              const ColumnVector &col,
                              BlockOffset offset,
                              BlockOffset row_count,
                              const HnswInsertConfig &config,
                              SizeT kBuildBucketSize) {
    SizeT mem_usage{};
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

SizeT HnswHandler::MemUsage() const {
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

SizeT HnswHandler::GetRowCount() const {
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

SizeT HnswHandler::GetSizeInBytes() const {
    return std::visit(
        [](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return SizeT(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->GetSizeInBytes();
                } else {
                    return SizeT(0);
                }
            }
        },
        hnsw_);
}

Pair<SizeT, SizeT> HnswHandler::GetInfo() const { return {MemUsage(), GetRowCount()}; }

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

void HnswHandler::SetLSGParam(float alpha, UniquePtr<float[]> avg) {
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

void HnswHandler::LoadFromPtr(LocalFileHandle &file_handle, SizeT file_size) {
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

void HnswHandler::LoadFromPtr(const char *ptr, SizeT size) {
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

HnswIndexInMem::~HnswIndexInMem() {
    SizeT mem_usage = hnsw_handler_->MemUsage();
    if (own_memory_ && hnsw_handler_ != nullptr) {
        delete hnsw_handler_;
    }
    if (trace_) {
        auto *storage = InfinityContext::instance().storage();
        if (storage == nullptr) {
            return;
        }
        auto *memindex_tracer = storage->memindex_tracer();
        if (memindex_tracer != nullptr) {
            memindex_tracer->DecreaseMemUsed(mem_usage);
        }
    }
}

UniquePtr<HnswIndexInMem> HnswIndexInMem::Make(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def, bool trace) {
    auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, trace);
    if (trace) {
        auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
        memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->MemUsage());
    }
    return memidx;
}

UniquePtr<HnswIndexInMem> HnswIndexInMem::Make(const IndexBase *index_base, const ColumnDef *column_def, bool trace) {
    RowID begin_row_id{0, 0};
    auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, trace);
    if (trace) {
        auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
        memindex_tracer->IncreaseMemoryUsage(memidx->hnsw_handler_->MemUsage());
    }
    return memidx;
}

MemIndexTracerInfo HnswIndexInMem::GetInfo() const {
    auto [mem_used, row_cnt] = hnsw_handler_->GetInfo();
    return MemIndexTracerInfo(MakeShared<String>(index_name_), MakeShared<String>(table_name_), MakeShared<String>(db_name_), mem_used, row_cnt);
}

void HnswIndexInMem::InsertVecs(SegmentOffset block_offset,
                                const ColumnVector &col,
                                BlockOffset offset,
                                BlockOffset row_count,
                                const HnswInsertConfig &config) {
    SizeT mem_usage = hnsw_handler_->InsertVecs(block_offset, col, offset, row_count, config, kBuildBucketSize);
    this->IncreaseMemoryUsageBase(mem_usage);
}

void HnswIndexInMem::Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr) {
    trace_ = false;
    if (dump_size_ptr != nullptr) {
        SizeT dump_size = hnsw_handler_->MemUsage();
        *dump_size_ptr = dump_size;
    }

    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<HnswHandlerPtr *>(handle.GetDataMut());
    *data_ptr = hnsw_handler_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
}

void HnswIndexInMem::SetLSGParam(float alpha, UniquePtr<float[]> avg) { hnsw_handler_->SetLSGParam(alpha, std::move(avg)); }

SizeT HnswIndexInMem::GetRowCount() const { return hnsw_handler_->GetRowCount(); }

SizeT HnswIndexInMem::GetSizeInBytes() const { return hnsw_handler_->GetSizeInBytes(); }

const ChunkIndexMetaInfo HnswIndexInMem::GetChunkIndexMetaInfo() const {
    return ChunkIndexMetaInfo{"", begin_row_id_, GetRowCount(), GetSizeInBytes()};
}

} // namespace infinity