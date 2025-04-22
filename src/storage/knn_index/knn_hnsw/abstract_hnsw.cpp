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

module abstract_hnsw;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;
import block_column_iter;
import segment_iter;
import segment_entry;
import table_index_entry;
import table_entry;
import memindex_tracer;
import default_values;
import logical_type;
import multivector_util;
import infinity_exception;
import third_party;

namespace infinity {

UniquePtr<HnswIndexInMem> HnswIndexInMem::Make(RowID begin_row_id,
                                               const IndexBase *index_base,
                                               const ColumnDef *column_def,
                                               SegmentIndexEntry *segment_index_entry,
                                               bool trace) {
    auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, segment_index_entry, trace);
    if (trace) {
        auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    if (index != nullptr) {
                        memindex_tracer->IncreaseMemoryUsage(index->mem_usage());
                    }
                }
            },
            memidx->hnsw_);
    }
    return memidx;
}

void HnswIndexInMem::SetSegmentEntry(SegmentIndexEntry *segment_index_entry) {
    segment_index_entry_ = segment_index_entry;
    begin_row_id_ = RowID(segment_index_entry->segment_id(), 0);
}

UniquePtr<HnswIndexInMem> HnswIndexInMem::Make(const IndexBase *index_base, const ColumnDef *column_def, bool trace) {
    RowID begin_row_id{0, 0};
    auto memidx = MakeUnique<HnswIndexInMem>(begin_row_id, index_base, column_def, nullptr, trace);
    if (trace) {
        auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    if (index != nullptr) {
                        memindex_tracer->IncreaseMemoryUsage(index->mem_usage());
                    }
                }
            },
            memidx->hnsw_);
    }
    return memidx;
}

HnswIndexInMem::HnswIndexInMem(RowID begin_row_id,
                               const IndexBase *index_base,
                               const ColumnDef *column_def,
                               SegmentIndexEntry *segment_index_entry,
                               bool trace)
    : begin_row_id_(begin_row_id), hnsw_(InitAbstractIndex(index_base, column_def)), segment_index_entry_(segment_index_entry), trace_(trace),
      own_memory_(true) {
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
                    UnrecoverableError("HnswIndexInMem::HnswIndexInMem: index does not own memory");
                }
            }
        },
        hnsw_);
}

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

AbstractHnsw HnswIndexInMem::InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    if (own_mem) {
        return InitAbstractIndexT<true>(index_base, column_def);
    } else {
        return InitAbstractIndexT<false>(index_base, column_def);
    }
}

HnswIndexInMem::~HnswIndexInMem() {
    SizeT mem_usage = 0;
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                mem_usage = arg->mem_usage();
                if (own_memory_) {
                    delete arg;
                }
            }
        },
        hnsw_);
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

SizeT HnswIndexInMem::GetRowCount() const {
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

SizeT HnswIndexInMem::GetSizeInBytes() const {
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

void HnswIndexInMem::InsertVecs(SizeT block_offset,
                                BlockColumnEntry *block_column_entry,
                                BufferManager *buffer_manager,
                                SizeT row_offset,
                                SizeT row_count,
                                const HnswInsertConfig &config) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using DataType = typename IndexT::DataType;
                    SizeT mem_usage{};
                    switch (const auto &column_data_type = block_column_entry->column_type(); column_data_type->type()) {
                        case LogicalType::kEmbedding: {
                            MemIndexInserterIter<DataType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
                            InsertVecs(index, std::move(iter), config, mem_usage);
                            break;
                        }
                        case LogicalType::kMultiVector: {
                            MemIndexInserterIter<MultiVectorRef<DataType>> iter(block_offset,
                                                                                block_column_entry,
                                                                                buffer_manager,
                                                                                row_offset,
                                                                                row_count);
                            InsertVecs(index, std::move(iter), config, mem_usage);
                            break;
                        }
                        default: {
                            UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                            break;
                        }
                    }
                    this->IncreaseMemoryUsageBase(mem_usage);
                } else {
                    UnrecoverableError("HnswIndexInMem::InsertVecs: index does not own memory");
                }
            }
        },
        hnsw_);
}

void HnswIndexInMem::InsertVecs(SegmentOffset block_offset,
                                const ColumnVector &col,
                                BlockOffset offset,
                                BlockOffset row_count,
                                const HnswInsertConfig &config) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using DataType = typename IndexT::DataType;
                    SizeT mem_usage{};
                    switch (const auto &column_data_type = col.data_type(); column_data_type->type()) {
                        case LogicalType::kEmbedding: {
                            // MemIndexInserterIter<DataType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
                            MemIndexInserterIter1<DataType> iter(block_offset, col, offset, row_count);
                            InsertVecs(index, std::move(iter), config, mem_usage);
                            break;
                        }
                        case LogicalType::kMultiVector: {
                            MemIndexInserterIter1<MultiVectorRef<DataType>> iter(block_offset, col, offset, row_count);
                            // MemIndexInserterIter<MultiVectorRef<DataType>> iter(block_offset,
                            //                                                     block_column_entry,
                            //                                                     buffer_manager,
                            //                                                     row_offset,
                            //                                                     row_count);
                            InsertVecs(index, std::move(iter), config, mem_usage);
                            break;
                        }
                        default: {
                            UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                            break;
                        }
                    }
                    this->IncreaseMemoryUsageBase(mem_usage);
                } else {
                    UnrecoverableError("HnswIndexInMem::InsertVecs: index does not own memory");
                }
            }
        },
        hnsw_);
}

void HnswIndexInMem::InsertVecs(const SegmentEntry *segment_entry,
                                BufferManager *buffer_mgr,
                                SizeT column_id,
                                TxnTimeStamp begin_ts,
                                bool check_ts,
                                const HnswInsertConfig &config) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (!IndexT::kOwnMem) {
                    UnrecoverableError("HnswIndexInMem::InsertVecs: index does not own memory");
                } else {
                    using DataType = typename IndexT::DataType;

                    SizeT mem_usage{};
                    switch (const auto &column_data_type = segment_entry->GetTableEntry()->GetColumnDefByID(column_id)->type();
                            column_data_type->type()) {
                        case LogicalType::kEmbedding: {
                            if (check_ts) {
                                OneColumnIterator<DataType> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                                InsertVecs(index, std::move(iter), config, mem_usage);
                            } else {
                                OneColumnIterator<DataType, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                                InsertVecs(index, std::move(iter), config, mem_usage);
                            }
                            break;
                        }
                        case LogicalType::kMultiVector: {
                            const auto ele_size = column_data_type->type_info()->Size();
                            if (check_ts) {
                                OneColumnIterator<MultiVectorRef<DataType>> iter(segment_entry, buffer_mgr, column_id, begin_ts, ele_size);
                                InsertVecs(index, std::move(iter), config, mem_usage);
                            } else {
                                OneColumnIterator<MultiVectorRef<DataType>, false> iter(segment_entry, buffer_mgr, column_id, begin_ts, ele_size);
                                InsertVecs(index, std::move(iter), config, mem_usage);
                            }
                            break;
                        }
                        default: {
                            UnrecoverableError(fmt::format("Unsupported column type for HNSW index: {}", column_data_type->ToString()));
                            break;
                        }
                    }
                    this->IncreaseMemoryUsageBase(mem_usage);
                }
            }
        },
        hnsw_);
}

void HnswIndexInMem::SetLSGParam(float alpha, UniquePtr<float[]> avg) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
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

SharedPtr<ChunkIndexEntry> HnswIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr, SizeT *dump_size_ptr) {
    SizeT row_count = 0;
    SizeT index_size = 0;
    SizeT dump_size = 0;
    trace_ = false;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                using IndexT = typename std::remove_pointer_t<T>;
                if constexpr (IndexT::kOwnMem) {
                    row_count = index->GetVecNum();
                    index_size = index->GetSizeInBytes();
                    dump_size = index->mem_usage();
                } else {
                    UnrecoverableError("HnswIndexInMem::Dump: index does not own memory");
                }
            }
        },
        hnsw_);
    auto new_chunk_indey_entry = segment_index_entry->CreateHnswIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);
    if (dump_size_ptr != nullptr) {
        *dump_size_ptr = dump_size;
    }

    BufferHandle handle = new_chunk_indey_entry->GetIndex();
    auto *data_ptr = static_cast<AbstractHnsw *>(handle.GetDataMut());
    *data_ptr = hnsw_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
    return new_chunk_indey_entry;
}

void HnswIndexInMem::Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr) {
    trace_ = false;
    if (dump_size_ptr != nullptr) {
        SizeT dump_size = 0;
        std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return;
                } else {
                    using IndexT = typename std::remove_pointer_t<T>;
                    if constexpr (IndexT::kOwnMem) {
                        dump_size = index->mem_usage();
                    } else {
                        UnrecoverableError("HnswIndexInMem::Dump: index does not own memory");
                    }
                }
            },
            hnsw_);
        *dump_size_ptr = dump_size;
    }

    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<AbstractHnsw *>(handle.GetDataMut());
    *data_ptr = hnsw_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
}

TableIndexEntry *HnswIndexInMem::table_index_entry() const { return segment_index_entry_->table_index_entry(); }

MemIndexTracerInfo HnswIndexInMem::GetInfo() const {
    auto [mem_used, row_cnt] = std::visit(
        [](auto &&index) -> Pair<SizeT, SizeT> {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return {};
            } else {
                return {index->mem_usage(), index->GetVecNum()};
            }
        },
        hnsw_);

    if (segment_index_entry_ == nullptr) {
        return MemIndexTracerInfo(MakeShared<String>(index_name_), MakeShared<String>(table_name_), MakeShared<String>(db_name_), mem_used, row_cnt);
    }

    auto *table_index_entry = segment_index_entry_->table_index_entry();
    SharedPtr<String> index_name = table_index_entry->GetIndexName();
    auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
    SharedPtr<String> table_name = table_entry->GetTableName();
    SharedPtr<String> db_name = table_entry->GetDBName();
    return MemIndexTracerInfo(index_name, table_name, db_name, mem_used, row_cnt);
}

} // namespace infinity