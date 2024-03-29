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

#include "type/complex/row_id.h"
#include <cassert>
#include <vector>

module segment_index_entry;

import stl;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import logger;
import third_party;
import infinity_exception;
import logical_type;
import index_file_worker;
import status;
import index_base;
import index_hnsw;
import hnsw_common;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import lvq_store;
import plain_store;
import catalog_delta_entry;
import column_vector;
import annivfflat_index_data;
import secondary_index_data;
import type_info;
import embedding_info;
import create_index_info;
import column_def;
import block_column_entry;
import default_values;
import segment_iter;
import annivfflat_index_file_worker;
import hnsw_file_worker;
import secondary_index_file_worker;
import index_full_text;
import index_defines;
import column_inverter;
import block_entry;
import local_file_system;
import column_length_io;

namespace infinity {

SegmentIndexEntry::SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer)
    : BaseEntry(EntryType::kSegmentIndex, false), table_index_entry_(table_index_entry), segment_id_(segment_id), vector_buffer_(std::move(vector_buffer)){};

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::CreateFakeEntry() {
    SharedPtr<SegmentIndexEntry> fake_entry;
    fake_entry.reset(new SegmentIndexEntry(static_cast<TableIndexEntry *>(nullptr), SegmentID(0), Vector<BufferObj *>()));
    return fake_entry;
}

SharedPtr<SegmentIndexEntry>
SegmentIndexEntry::NewIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Txn *txn, CreateIndexParam *param) {
    auto *buffer_mgr = txn->buffer_mgr();

    // FIXME: estimate index size.
    auto vector_file_worker = table_index_entry->CreateFileWorker(param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_mgr->Allocate(std::move(vector_file_worker[i]));
    };
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
    auto begin_ts = txn->BeginTS();
    segment_index_entry->min_ts_ = begin_ts;
    segment_index_entry->max_ts_ = begin_ts;
    segment_index_entry->begin_ts_ = begin_ts;

    return segment_index_entry;
}

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::NewReplaySegmentIndexEntry(TableIndexEntry *table_index_entry,
                                                                           TableEntry *table_entry,
                                                                           SegmentID segment_id,
                                                                           BufferManager *buffer_manager,
                                                                           TxnTimeStamp min_ts,
                                                                           TxnTimeStamp max_ts,
                                                                           TransactionID txn_id,
                                                                           TxnTimeStamp begin_ts,
                                                                           TxnTimeStamp commit_ts,
                                                                           bool is_delete) {

    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    String column_name = table_index_entry->index_base()->column_name();
    auto create_index_param = SegmentIndexEntry::GetCreateIndexParam(table_index_entry->table_index_def(),
                                                                     segment_row_count,
                                                                     table_entry->GetColumnDefByName(column_name));
    auto vector_file_worker = table_index_entry->CreateFileWorker(create_index_param.get(), segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->Get(std::move(vector_file_worker[i]));
    };
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
    if (segment_index_entry.get() == nullptr) {
        UnrecoverableError("Failed to load index entry");
    }
    segment_index_entry->min_ts_ = min_ts;
    segment_index_entry->max_ts_ = max_ts;
    segment_index_entry->commit_ts_.store(commit_ts);
    return segment_index_entry;
}

UniquePtr<SegmentIndexEntry>
SegmentIndexEntry::LoadIndexEntry(TableIndexEntry *table_index_entry, u32 segment_id, BufferManager *buffer_manager, CreateIndexParam *param) {
    auto vector_file_worker = table_index_entry->CreateFileWorker(param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->Get(std::move(vector_file_worker[i]));
    }
    return UniquePtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
}

BufferHandle SegmentIndexEntry::GetIndex() { return vector_buffer_[0]->Load(); }

BufferHandle SegmentIndexEntry::GetIndexPartAt(u32 idx) { return vector_buffer_[idx + 1]->Load(); }

void SegmentIndexEntry::MemIndexInsert(Txn *txn, SharedPtr<BlockEntry> block_entry, u32 row_offset, u32 row_count) {
    u32 seg_id = block_entry->segment_id();
    u16 block_id = block_entry->block_id();
    RowID begin_row_id(seg_id, row_offset + u32(block_id) * block_entry->row_capacity());

    const IndexBase *index_base = table_index_entry_->index_base();
    const ColumnDef *column_def = table_index_entry_->column_def().get();
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
            if (memory_indexer_.get() == nullptr) {
                String base_name = fmt::format("ft_{}", begin_row_id.ToUint64());
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry_->index_dir(),
                                                            base_name,
                                                            begin_row_id,
                                                            index_fulltext->flag_,
                                                            index_fulltext->analyzer_,
                                                            table_index_entry_->GetFulltextByteSlicePool(),
                                                            table_index_entry_->GetFulltextBufferPool(),
                                                            table_index_entry_->GetFulltextThreadPool());
                auto now = Clock::now().time_since_epoch();
                u64 ts = ChronoCast<NanoSeconds>(now).count();
                table_index_entry_->UpdateFulltextSegmentTs(ts);
            }
            Path column_length_file_base = Path(*table_index_entry_->index_dir()) / (memory_indexer_->GetBaseName());
            String column_length_file_path_prefix = column_length_file_base.string();
            String column_length_file_path = column_length_file_path_prefix + LENGTH_SUFFIX;
            auto column_length_file_handler =
                MakeShared<FullTextColumnLengthFileHandler>(MakeUnique<LocalFileSystem>(), column_length_file_path, this);
            u64 column_id = column_def->id();
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
            SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetColumnVector(txn->buffer_mgr()));
            memory_indexer_->Insert(column_vector, row_offset, row_count, std::move(column_length_file_handler), false);
            break;
        }
        case IndexType::kIVFFlat:
        case IndexType::kHnsw:
        case IndexType::kSecondary: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("{} realtime index is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_WARN(*err_msg);
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
}

void SegmentIndexEntry::MemIndexCommit() {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText || memory_indexer_.get() == nullptr)
        return;
    memory_indexer_->Commit();
}

void SegmentIndexEntry::MemIndexDump(bool spill) {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText || memory_indexer_.get() == nullptr)
        return;
    memory_indexer_->Dump(false, spill);
    if (!spill) {
        std::unique_lock<std::shared_mutex> lck(rw_locker_);
        ft_base_names_.push_back(memory_indexer_->GetBaseName());
        ft_base_rowids_.push_back(memory_indexer_->GetBaseRowId());
        memory_indexer_.reset();
        auto now = Clock::now().time_since_epoch();
        u64 ts = ChronoCast<NanoSeconds>(now).count();
        table_index_entry_->UpdateFulltextSegmentTs(ts);
    }
}

void SegmentIndexEntry::MemIndexLoad(const String &base_name, RowID base_row_id) {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText)
        return;
    // Init the mem index from previously spilled one.
    assert(memory_indexer_.get() == nullptr);
    const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
    memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry_->index_dir(),
                                                base_name,
                                                base_row_id,
                                                index_fulltext->flag_,
                                                index_fulltext->analyzer_,
                                                table_index_entry_->GetFulltextByteSlicePool(),
                                                table_index_entry_->GetFulltextBufferPool(),
                                                table_index_entry_->GetFulltextThreadPool());
    memory_indexer_->Load();
}

void SegmentIndexEntry::PopulateEntirely(const SegmentEntry *segment_entry, Txn *txn) {
    auto *buffer_mgr = txn->buffer_mgr();
    const IndexBase *index_base = table_index_entry_->index_base();
    const ColumnDef *column_def = table_index_entry_->column_def().get();
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
            u32 seg_id = segment_entry->segment_id();
            RowID base_row_id(seg_id, 0);
            String base_name = fmt::format("ft_{}", base_row_id.ToUint64());
            memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry_->index_dir(),
                                                        base_name,
                                                        base_row_id,
                                                        index_fulltext->flag_,
                                                        index_fulltext->analyzer_,
                                                        table_index_entry_->GetFulltextByteSlicePool(),
                                                        table_index_entry_->GetFulltextBufferPool(),
                                                        table_index_entry_->GetFulltextThreadPool());
            u64 column_id = column_def->id();
            Path column_length_file_base = Path(*table_index_entry_->index_dir()) / base_name;
            String column_length_file_path_prefix = column_length_file_base.string();
            String column_length_file_path = column_length_file_path_prefix + LENGTH_SUFFIX;
            auto column_length_file_handler =
                MakeShared<FullTextColumnLengthFileHandler>(MakeUnique<LocalFileSystem>(), column_length_file_path, this);
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetColumnVector(buffer_mgr));
                memory_indexer_->Insert(column_vector, 0, block_entry->row_count(), column_length_file_handler, true);
                memory_indexer_->Commit(true);
            }
            column_length_file_handler.reset();
            memory_indexer_->Dump(true);
            {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                ft_base_names_.push_back(base_name);
                ft_base_rowids_.push_back(base_row_id);
                memory_indexer_.reset();
            }
            auto duration = Clock::now().time_since_epoch();
            u64 ts = ChronoCast<NanoSeconds>(duration).count();
            table_index_entry_->UpdateFulltextSegmentTs(ts);
            break;
        }
        case IndexType::kIVFFlat:
        case IndexType::kHnsw:
        case IndexType::kSecondary: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("{} PopulateEntirely is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_WARN(*err_msg);
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
}

Status SegmentIndexEntry::CreateIndexPrepare(const SegmentEntry *segment_entry, Txn *txn, bool prepare, bool check_ts) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    auto *buffer_mgr = txn->buffer_mgr();
    const IndexBase *index_base = table_index_entry_->index_base();
    const ColumnDef *column_def = table_index_entry_->column_def().get();
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("AnnIVFFlat only supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);
            u32 dimension = embedding_info->Dimension();
            u32 full_row_count = segment_entry->row_count();
            BufferHandle buffer_handle = GetIndex();
            switch (embedding_info->Type()) {
                case kElemFloat: {
                    auto annivfflat_index = reinterpret_cast<AnnIVFFlatIndexData<f32> *>(buffer_handle.GetDataMut());
                    // TODO: How to select training data?
                    if (check_ts) {
                        OneColumnIterator<float> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                        annivfflat_index->BuildIndex(iter, dimension, full_row_count);
                    } else {
                        // Not check ts in uncommitted segment when compact segment
                        OneColumnIterator<float, false> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                        annivfflat_index->BuildIndex(iter, dimension, full_row_count);
                    }
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not support data type for index ivf."));
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BufferHandle buffer_handle = GetIndex();

            auto InsertHnsw = [&](auto &hnsw_index) {
                auto InsertHnswInner = [&](auto &iter) {
                    if (!prepare) {
                        // Single thread insert
                        hnsw_index->InsertVecs(iter, segment_entry->row_count()); // estimate insert count
                    } else {
                        // Multi thread insert data, write file in the physical create index finish stage.
                        hnsw_index->StoreData(iter, segment_entry->row_count());
                    }
                };
                if (check_ts) {
                    OneColumnIterator<float> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                    InsertHnswInner(iter);
                } else {
                    // Not check ts in uncommitted segment when compact segment
                    OneColumnIterator<float, false> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                    InsertHnswInner(iter);
                }
            };

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    switch (index_hnsw->encode_type_) {
                        case HnswEncodeType::kPlain: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainIPDist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainL2Dist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Invalid metric type.");
                                }
                            }
                            break;
                        }
                        case HnswEncodeType::kLVQ: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    // too long type. fix it.
                                    auto hnsw_index = static_cast<KnnHnsw<float,
                                                                          SegmentOffset,
                                                                          LVQStore<float, SegmentOffset, i8, LVQIPCache<float, i8>>,
                                                                          LVQIPDist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto hnsw_index = static_cast<KnnHnsw<float,
                                                                          SegmentOffset,
                                                                          LVQStore<float, SegmentOffset, i8, LVQL2Cache<float, i8>>,
                                                                          LVQL2Dist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnsw(hnsw_index);
                                    break;
                                }
                                default: {
                                    UnrecoverableError("Invalid metric type.");
                                }
                            }
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid HNSW encode type.");
                        }
                    }
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not support data type for index hnsw."));
                }
            }
            break;
        }
        case IndexType::kFullText: {
            PopulateEntirely(segment_entry, txn);
            break;
        }
        case IndexType::kSecondary: {
            auto &data_type = column_def->type();
            if (!(data_type->CanBuildSecondaryIndex())) {
                UnrecoverableError(fmt::format("Cannot build secondary index on data type: {}", data_type->ToString()));
            }
            // 1. build secondary index by merge sort
            u32 part_capacity = DEFAULT_BLOCK_CAPACITY;
            // fetch the row_count from segment_entry
            auto secondary_index_builder = GetSecondaryIndexDataBuilder(data_type, segment_entry->row_count(), part_capacity);
            secondary_index_builder->LoadSegmentData(segment_entry, buffer_mgr, column_def->id(), begin_ts, check_ts);
            secondary_index_builder->StartOutput();
            // 2. output into SecondaryIndexDataPart
            {
                u32 part_num = GetIndexPartNum();
                for (u32 part_id = 0; part_id < part_num; ++part_id) {
                    BufferHandle buffer_handle_part = GetIndexPartAt(part_id);
                    auto secondary_index_part = static_cast<SecondaryIndexDataPart *>(buffer_handle_part.GetDataMut());
                    secondary_index_builder->OutputToPart(secondary_index_part);
                }
            }
            // 3. output into SecondaryIndexDataHead
            {
                BufferHandle buffer_handle_head = GetIndex();
                auto secondary_index_head = static_cast<SecondaryIndexDataHead *>(buffer_handle_head.GetDataMut());
                secondary_index_builder->OutputToHeader(secondary_index_head);
            }
            secondary_index_builder->EndOutput();
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    return Status::OK();
}

Status SegmentIndexEntry::CreateIndexDo(atomic_u64 &create_index_idx) {
    const IndexBase *index_base = table_index_entry_->index_base();
    const ColumnDef *column_def = table_index_entry_->column_def().get();
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            auto InsertHnswDo = [&](auto *hnsw_index, atomic_u64 &create_index_idx) {
                SizeT vertex_n = hnsw_index->GetVertexNum();
                while (true) {
                    SizeT idx = create_index_idx.fetch_add(1);
                    if (idx % 10000 == 0) {
                        LOG_TRACE(fmt::format("Insert index: {}", idx));
                    }
                    if (idx >= vertex_n) {
                        break;
                    }
                    hnsw_index->Build(idx);
                }
            };
            auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BufferHandle buffer_handle = GetIndex();

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    switch (index_hnsw->encode_type_) {
                        case HnswEncodeType::kPlain: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto *hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainIPDist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index = static_cast<
                                        KnnHnsw<float, SegmentOffset, PlainStore<float, SegmentOffset>, PlainL2Dist<float, SegmentOffset>> *>(
                                        buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                default: {
                                    RecoverableError(Status::NotSupport("Not implemented"));
                                }
                            }
                            break;
                        }
                        case HnswEncodeType::kLVQ: {
                            switch (index_hnsw->metric_type_) {
                                case MetricType::kMerticInnerProduct: {
                                    auto *hnsw_index = static_cast<KnnHnsw<float,
                                                                           SegmentOffset,
                                                                           LVQStore<float, SegmentOffset, i8, LVQIPCache<float, i8>>,
                                                                           LVQIPDist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                case MetricType::kMerticL2: {
                                    auto *hnsw_index = static_cast<KnnHnsw<float,
                                                                           SegmentOffset,
                                                                           LVQStore<float, SegmentOffset, i8, LVQL2Cache<float, i8>>,
                                                                           LVQL2Dist<float, SegmentOffset, i8>> *>(buffer_handle.GetDataMut());
                                    InsertHnswDo(hnsw_index, create_index_idx);
                                    break;
                                }
                                default: {
                                    RecoverableError(Status::NotSupport("Not implemented"));
                                }
                            }
                            break;
                        }
                        default: {
                            RecoverableError(Status::NotSupport("Not implemented"));
                        }
                    }
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not implemented"));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    return Status::OK();
}

bool SegmentIndexEntry::Flush(TxnTimeStamp checkpoint_ts) {
    if (table_index_entry_->index_base()->index_type_ == IndexType::kFullText) {
        // Fulltext index doesn't need to be checkpointed.
        return false;
    }

    String &index_name = *table_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    if (this->max_ts_ <= this->checkpoint_ts_ || this->min_ts_ > checkpoint_ts) {
        LOG_TRACE(fmt::format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                              segment_id,
                              index_name));
        return false;
    }

    this->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} checkpoint is change to {}", segment_id, index_name, this->checkpoint_ts_));
    return true;
}

void SegmentIndexEntry::Cleanup() {
    for (auto *buffer_obj : vector_buffer_) {
        if (buffer_obj == nullptr) {
            UnrecoverableError("vector_buffer should not has nullptr.");
        }
        buffer_obj->SetAndTryCleanup();
    }
}

void SegmentIndexEntry::PickCleanup(CleanupScanner *scanner) {}

UniquePtr<CreateIndexParam> SegmentIndexEntry::GetCreateIndexParam(SharedPtr<IndexBase> index_base, SizeT seg_row_count, SharedPtr<ColumnDef> column_def) {
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            return MakeUnique<CreateAnnIVFFlatParam>(index_base, column_def, seg_row_count);
        }
        case IndexType::kHnsw: {
            SizeT max_element = seg_row_count;
            return MakeUnique<CreateHnswParam>(index_base, column_def, max_element);
        }
        case IndexType::kFullText: {
            return MakeUnique<CreateIndexParam>(index_base, column_def);
        }
        case IndexType::kSecondary: {
            u32 part_capacity = DEFAULT_BLOCK_CAPACITY;
            return MakeUnique<CreateSecondaryIndexParam>(index_base, column_def, seg_row_count, part_capacity);
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    return nullptr;
}

void SegmentIndexEntry::SaveIndexFile() {
    String &index_name = *table_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    for (auto &buffer_ptr : vector_buffer_) {
        if (buffer_ptr->Save()) {
            buffer_ptr->Sync();
            buffer_ptr->CloseFile();
        }
    }
}

nlohmann::json SegmentIndexEntry::Serialize() {
    if (this->deleted_) {
        UnrecoverableError("Segment Column index entry can't be deleted.");
    }

    nlohmann::json index_entry_json;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        index_entry_json["segment_id"] = this->segment_id_;
        index_entry_json["min_ts"] = this->min_ts_;
        index_entry_json["max_ts"] = this->max_ts_;
        index_entry_json["checkpoint_ts"] = this->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry
        index_entry_json["ft_base_names"] = this->ft_base_names_;
        Vector<u64> base_row_ids(this->ft_base_rowids_.size());
        for (SizeT i = 0; i < this->ft_base_rowids_.size(); i++) {
            base_row_ids[i] = this->ft_base_rowids_[i].ToUint64();
        }
        index_entry_json["ft_base_rowids"] = base_row_ids;
        if (this->memory_indexer_.get() != nullptr) {
            index_entry_json["ft_mem_base_name"] = this->memory_indexer_->GetBaseName();
            index_entry_json["ft_mem_base_rowid"] = this->memory_indexer_->GetBaseRowId().ToUint64();
        }
        index_entry_json["ft_column_len_sum"] = this->ft_column_len_sum_;
        index_entry_json["ft_column_len_cnt"] = this->ft_column_len_cnt_;
    }

    return index_entry_json;
}

UniquePtr<SegmentIndexEntry> SegmentIndexEntry::Deserialize(const nlohmann::json &index_entry_json,
                                                            TableIndexEntry *table_index_entry,
                                                            BufferManager *buffer_mgr,
                                                            TableEntry *table_entry) {
    u32 segment_id = index_entry_json["segment_id"];
    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);

    if (!status.ok()) {
        UnrecoverableError(status.message());
        return nullptr;
    }
    const IndexBase *index_base = table_index_entry->index_base();
    String column_name = index_base->column_name();
    UniquePtr<CreateIndexParam> create_index_param =
        GetCreateIndexParam(table_index_entry->table_index_def(), segment_row_count, table_entry->GetColumnDefByName(column_name));
    auto segment_index_entry = LoadIndexEntry(table_index_entry, segment_id, buffer_mgr, create_index_param.get());
    if (segment_index_entry.get() == nullptr) {
        UnrecoverableError("Failed to load index entry");
    }
    segment_index_entry->min_ts_ = index_entry_json["min_ts"];
    segment_index_entry->max_ts_ = index_entry_json["max_ts"];
    segment_index_entry->checkpoint_ts_ = index_entry_json["checkpoint_ts"]; // TODO shenyushi:: use fields in BaseEntry
    index_entry_json["ft_base_names"].get_to<Vector<String>>(segment_index_entry->ft_base_names_);
    Vector<u64> base_row_ids;
    index_entry_json["ft_base_rowids"].get_to<Vector<u64>>(base_row_ids);
    segment_index_entry->ft_base_rowids_.resize(base_row_ids.size());
    for (SizeT i = 0; i < base_row_ids.size(); i++) {
        segment_index_entry->ft_base_rowids_[i] = RowID::FromUint64(base_row_ids[i]);
    }
    if (index_entry_json.contains("ft_mem_base_name")) {
        String base_name = index_entry_json["ft_mem_base_name"];
        RowID base_row_id = RowID::FromUint64(index_entry_json["ft_mem_base_rowid"]);
        const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
        segment_index_entry->memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry->index_dir(),
                                                                         base_name,
                                                                         base_row_id,
                                                                         index_fulltext->flag_,
                                                                         index_fulltext->analyzer_,
                                                                         table_index_entry->GetFulltextByteSlicePool(),
                                                                         table_index_entry->GetFulltextBufferPool(),
                                                                         table_index_entry->GetFulltextThreadPool());
        segment_index_entry->memory_indexer_->Load();
    }
    segment_index_entry->ft_column_len_sum_ = index_entry_json["ft_column_len_sum"];
    segment_index_entry->ft_column_len_cnt_ = index_entry_json["ft_column_len_cnt"];
    return segment_index_entry;
}

} // namespace infinity
