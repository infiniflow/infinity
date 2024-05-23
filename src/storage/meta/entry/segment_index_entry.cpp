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

#include <cassert>
#include <sstream>
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
import hnsw_alg;
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
import chunk_index_entry;
import abstract_hnsw;
import block_column_iter;
import txn_store;
import secondary_index_in_mem;

namespace infinity {

Vector<std::string_view> SegmentIndexEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        UnrecoverableError(fmt::format("Invalid segment index entry encode: {}", encode));
    }
    auto decodes = TableIndexEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String SegmentIndexEntry::EncodeIndex(const SegmentID segment_id, const TableIndexEntry *table_index_entry) {
    if (table_index_entry == nullptr) {
        return ""; // unit test
    }
    return fmt::format("{}#{}", table_index_entry->encode(), segment_id);
}

SegmentIndexEntry::SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer)
    : BaseEntry(EntryType::kSegmentIndex, false, SegmentIndexEntry::EncodeIndex(segment_id, table_index_entry)),
      table_index_entry_(table_index_entry), segment_id_(segment_id), vector_buffer_(std::move(vector_buffer)) {
    if (table_index_entry != nullptr)
        index_dir_ = table_index_entry->index_dir();
};

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::CreateFakeEntry(const String &index_dir) {
    SharedPtr<SegmentIndexEntry> fake_entry;
    fake_entry.reset(new SegmentIndexEntry(static_cast<TableIndexEntry *>(nullptr), SegmentID(0), Vector<BufferObj *>()));
    fake_entry->index_dir_ = MakeShared<String>(index_dir);
    return fake_entry;
}

SharedPtr<SegmentIndexEntry>
SegmentIndexEntry::NewIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Txn *txn, CreateIndexParam *param) {
    auto *buffer_mgr = txn->buffer_mgr();

    // FIXME: estimate index size.
    auto vector_file_worker = SegmentIndexEntry::CreateFileWorkers(table_index_entry->index_dir(), param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_mgr->AllocateBufferObject(std::move(vector_file_worker[i]));
    };
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
    auto begin_ts = txn->BeginTS();
    segment_index_entry->min_ts_ = begin_ts;
    segment_index_entry->max_ts_ = begin_ts;
    segment_index_entry->begin_ts_ = begin_ts;
    segment_index_entry->buffer_manager_ = buffer_mgr;

    return segment_index_entry;
}

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::NewReplaySegmentIndexEntry(TableIndexEntry *table_index_entry,
                                                                           TableEntry *table_entry,
                                                                           SegmentID segment_id,
                                                                           BufferManager *buffer_manager,
                                                                           TxnTimeStamp min_ts,
                                                                           TxnTimeStamp max_ts,
                                                                           u32 next_chunk_id,
                                                                           TransactionID txn_id,
                                                                           TxnTimeStamp begin_ts,
                                                                           TxnTimeStamp commit_ts) {
    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    String column_name = table_index_entry->index_base()->column_name();
    auto create_index_param =
        SegmentIndexEntry::GetCreateIndexParam(table_index_entry->table_index_def(), segment_row_count, table_entry->GetColumnDefByName(column_name));
    auto vector_file_worker = SegmentIndexEntry::CreateFileWorkers(table_index_entry->index_dir(), create_index_param.get(), segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->GetBufferObject(std::move(vector_file_worker[i]));
    };
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
    if (segment_index_entry.get() == nullptr) {
        UnrecoverableError("Failed to load index entry");
    }
    segment_index_entry->min_ts_ = min_ts;
    segment_index_entry->max_ts_ = max_ts;
    segment_index_entry->next_chunk_id_ = next_chunk_id;
    {
        LocalFileSystem fs;
        for (ChunkID chunk_id = next_chunk_id;; ++chunk_id) {
            String chunk_file_name = ChunkIndexEntry::IndexFileName(segment_id, chunk_id);
            String file_path = *table_index_entry->index_dir() + "/" + chunk_file_name;
            if (!fs.Exists(file_path)) {
                break;
            }
            fs.DeleteFile(file_path);
        }
    }
    segment_index_entry->commit_ts_.store(commit_ts);
    segment_index_entry->buffer_manager_ = buffer_manager;
    return segment_index_entry;
}

Vector<UniquePtr<IndexFileWorker>> SegmentIndexEntry::CreateFileWorkers(SharedPtr<String> index_dir, CreateIndexParam *param, SegmentID segment_id) {
    Vector<UniquePtr<IndexFileWorker>> vector_file_worker;
    // reference file_worker will be invalidated when vector_file_worker is resized
    const auto index_base = param->index_base_;
    const auto column_def = param->column_def_;
    switch (index_base->index_type_) {
        case IndexType::kHnsw:
        case IndexType::kFullText:
        case IndexType::kSecondary: {
            // these indexes don't use BufferManager
            return vector_file_worker;
        }
        default: {
            break;
        }
    }

    auto file_name = MakeShared<String>(IndexFileName(segment_id));
    vector_file_worker.resize(1);
    auto &file_worker = vector_file_worker[0];
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            auto create_annivfflat_param = static_cast<CreateAnnIVFFlatParam *>(param);
            auto elem_type = ((EmbeddingInfo *)(column_def->type()->type_info().get()))->Type();
            switch (elem_type) {
                case kElemFloat: {
                    file_worker =
                        MakeUnique<AnnIVFFlatIndexFileWorker<f32>>(index_dir, file_name, index_base, column_def, create_annivfflat_param->row_count_);
                    break;
                }
                default: {
                    UnrecoverableError("Create IVF Flat index: Unsupported element type.");
                }
            }
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("File worker isn't implemented: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_ERROR(*err_msg);
            UnrecoverableError(*err_msg);
        }
    }
    // cannot use invalid file_worker
    if (vector_file_worker[0].get() == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>("Failed to create index file worker");
        LOG_ERROR(*err_msg);
        UnrecoverableError(*err_msg);
    }
    return vector_file_worker;
}

String SegmentIndexEntry::IndexFileName(SegmentID segment_id) { return fmt::format("seg{}.idx", segment_id); }

UniquePtr<SegmentIndexEntry>
SegmentIndexEntry::LoadIndexEntry(TableIndexEntry *table_index_entry, u32 segment_id, BufferManager *buffer_manager, CreateIndexParam *param) {
    auto vector_file_worker = SegmentIndexEntry::CreateFileWorkers(table_index_entry->index_dir(), param, segment_id);
    Vector<BufferObj *> vector_buffer(vector_file_worker.size());
    for (u32 i = 0; i < vector_file_worker.size(); ++i) {
        vector_buffer[i] = buffer_manager->GetBufferObject(std::move(vector_file_worker[i]));
    }
    return UniquePtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id, std::move(vector_buffer)));
}

BufferHandle SegmentIndexEntry::GetIndex() { return vector_buffer_[0]->Load(); }

BufferHandle SegmentIndexEntry::GetIndexPartAt(u32 idx) { return vector_buffer_[idx + 1]->Load(); }

void SegmentIndexEntry::MemIndexInsert(SharedPtr<BlockEntry> block_entry,
                                       u32 row_offset,
                                       u32 row_count,
                                       TxnTimeStamp commit_ts,
                                       BufferManager *buffer_manager) {
    SegmentOffset block_offset = block_entry->segment_offset();
    RowID begin_row_id = block_entry->base_row_id() + row_offset;

    const SharedPtr<IndexBase> &index_base = table_index_entry_->table_index_def();
    const SharedPtr<ColumnDef> &column_def = table_index_entry_->column_def();
    ColumnID column_id = column_def->id();

    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base.get());
            if (memory_indexer_.get() == nullptr) {
                String base_name = fmt::format("ft_{:016x}", begin_row_id.ToUint64());
                {
                    std::unique_lock<std::shared_mutex> lck(rw_locker_);
                    memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry_->index_dir(),
                                                                base_name,
                                                                begin_row_id,
                                                                index_fulltext->flag_,
                                                                index_fulltext->analyzer_,
                                                                table_index_entry_->GetFulltextByteSlicePool(),
                                                                table_index_entry_->GetFulltextBufferPool(),
                                                                table_index_entry_->GetFulltextInvertingThreadPool(),
                                                                table_index_entry_->GetFulltextCommitingThreadPool());
                }
                table_index_entry_->UpdateFulltextSegmentTs(commit_ts);
            } else {
                RowID exp_begin_row_id = memory_indexer_->GetBaseRowId() + memory_indexer_->GetDocCount();
                assert(begin_row_id >= exp_begin_row_id);
                if (begin_row_id > exp_begin_row_id) {
                    LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                         begin_row_id.ToUint64(),
                                         exp_begin_row_id.ToUint64(),
                                         begin_row_id - exp_begin_row_id));
                    memory_indexer_->InsertGap(begin_row_id - exp_begin_row_id);
                }
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
            SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetColumnVector(buffer_manager));
            memory_indexer_->Insert(column_vector, row_offset, row_count, false);
            break;
        }
        case IndexType::kHnsw: {
            const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base.get());
            if (memory_hnsw_indexer_.get() == nullptr) {
                SharedPtr<ChunkIndexEntry> memory_hnsw_indexer = CreateChunkIndexEntry(column_def, begin_row_id, buffer_manager);

                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_hnsw_indexer_ = std::move(memory_hnsw_indexer);
            }
            BufferHandle buffer_handle = memory_hnsw_indexer_->GetIndex();

            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
            SizeT row_cnt = 0;
            switch (embedding_info->Type()) {
                case kElemFloat: {
                    AbstractHnsw<f32, SegmentOffset> abstract_hnsw(buffer_handle.GetDataMut(), index_hnsw);
                    MemIndexInserterIter<f32> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
                    auto [start_i, end_i] = abstract_hnsw.InsertVecs(std::move(iter));
                    row_cnt = end_i;
                    break;
                }
                default: {
                    Status status = Status::NotSupport("Not support data type for index hnsw.");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
            }
            memory_hnsw_indexer_->SetRowCount(row_cnt);
            break;
        }
        case IndexType::kSecondary: {
            if (memory_secondary_index_.get() == nullptr) {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, begin_row_id);
            }
            auto block_id = block_entry->block_id();
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
            memory_secondary_index_->Insert(block_id, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kIVFFlat: {
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
    assert(commit_ts >= min_ts_);
    max_ts_ = commit_ts;
}

void SegmentIndexEntry::MemIndexCommit() {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText || memory_indexer_.get() == nullptr)
        return;
    memory_indexer_->Commit();
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::MemIndexDump(bool spill) {
    SharedPtr<ChunkIndexEntry> chunk_index_entry = nullptr;
    const IndexBase *index_base = table_index_entry_->index_base();
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            if (memory_hnsw_indexer_.get() == nullptr) {
                return nullptr;
            }
            auto dump_indexer = std::exchange(memory_hnsw_indexer_, nullptr);
            this->AddChunkIndexEntry(dump_indexer);
            return dump_indexer;
        }
        case IndexType::kFullText: {
            if (memory_indexer_.get() == nullptr) {
                return nullptr;
            }
            memory_indexer_->Dump(false, spill);
            if (!spill) {
                chunk_index_entry =
                    AddFtChunkIndexEntry(memory_indexer_->GetBaseName(), memory_indexer_->GetBaseRowId(), memory_indexer_->GetDocCount());
                this->UpdateFulltextColumnLenInfo(memory_indexer_->GetColumnLengthSum(), memory_indexer_->GetDocCount());
                memory_indexer_.reset();
            }
            return chunk_index_entry;
        }
        case IndexType::kSecondary: {
            if (memory_secondary_index_.get() == nullptr) {
                return nullptr;
            }
            std::unique_lock lck(rw_locker_);
            SharedPtr<ChunkIndexEntry> chunk_index_entry = memory_secondary_index_->Dump(this, buffer_manager_);
            chunk_index_entry->SaveIndexFile();
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_secondary_index_.reset();
            return chunk_index_entry;
        }
        default: {
            return nullptr;
        }
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
                                                table_index_entry_->GetFulltextInvertingThreadPool(),
                                                table_index_entry_->GetFulltextCommitingThreadPool());
    memory_indexer_->Load();
}

u32 SegmentIndexEntry::MemIndexRowCount() {
    const IndexBase *index_base = table_index_entry_->index_base();
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            return memory_indexer_.get() ? memory_indexer_->GetDocCount() : 0;
        }
        case IndexType::kHnsw: {
            return memory_hnsw_indexer_.get() ? memory_hnsw_indexer_->GetRowCount() : 0;
        }
        case IndexType::kSecondary: {
            return memory_secondary_index_.get() ? memory_secondary_index_->GetRowCount() : 0;
        }
        default: {
            return 0;
        }
    }
}

void SegmentIndexEntry::PopulateEntirely(const SegmentEntry *segment_entry, Txn *txn, const PopulateEntireConfig &config) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    auto *buffer_mgr = txn->buffer_mgr();
    const IndexBase *index_base = table_index_entry_->index_base();
    SharedPtr<ColumnDef> column_def = table_index_entry_->column_def();
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
            u32 seg_id = segment_entry->segment_id();
            RowID base_row_id(seg_id, 0);
            String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
            memory_indexer_ = MakeUnique<MemoryIndexer>(*table_index_entry_->index_dir(),
                                                        base_name,
                                                        base_row_id,
                                                        index_fulltext->flag_,
                                                        index_fulltext->analyzer_,
                                                        table_index_entry_->GetFulltextByteSlicePool(),
                                                        table_index_entry_->GetFulltextBufferPool(),
                                                        table_index_entry_->GetFulltextInvertingThreadPool(),
                                                        table_index_entry_->GetFulltextCommitingThreadPool());
            u64 column_id = column_def->id();
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                RowID begin_row_id = block_entry->base_row_id();
                RowID exp_begin_row_id = memory_indexer_->GetBaseRowId() + memory_indexer_->GetDocCount();
                assert(begin_row_id >= exp_begin_row_id);
                if (begin_row_id > exp_begin_row_id) {
                    LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                         begin_row_id.ToUint64(),
                                         exp_begin_row_id.ToUint64(),
                                         begin_row_id - exp_begin_row_id));
                    memory_indexer_->InsertGap(begin_row_id - exp_begin_row_id);
                }

                SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetColumnVector(buffer_mgr));
                memory_indexer_->Insert(column_vector, 0, block_entry->row_count(), true);
                memory_indexer_->Commit(true);
            }
            memory_indexer_->Dump(true);
            AddFtChunkIndexEntry(memory_indexer_->GetBaseName(), memory_indexer_->GetBaseRowId(), memory_indexer_->GetDocCount());
            this->UpdateFulltextColumnLenInfo(memory_indexer_->GetColumnLengthSum(), memory_indexer_->GetDocCount());
            memory_indexer_.reset();
            break;
        }
        case IndexType::kHnsw: {
            auto index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            RowID base_rowid(segment_entry->segment_id(), 0);
            SharedPtr<ChunkIndexEntry> chunk_index_entry = CreateChunkIndexEntry(column_def, base_rowid, buffer_mgr);
            this->AddChunkIndexEntry(chunk_index_entry);
            BufferHandle buffer_handle = chunk_index_entry->GetIndex();

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    AbstractHnsw<f32, SegmentOffset> abstract_hnsw(buffer_handle.GetDataMut(), index_hnsw);
                    auto InsertHnswInner = [&](auto &iter) {
                        HnswInsertConfig insert_config;
                        insert_config.optimize_ = true;
                        SegmentOffset start_i, end_i;
                        if (!config.prepare_) {
                            // Single thread insert
                            std::tie(start_i, end_i) = abstract_hnsw.InsertVecs(std::move(iter), insert_config);
                        } else {
                            // Multi thread insert data, write file in the physical create index finish stage.
                            std::tie(start_i, end_i) = abstract_hnsw.StoreData(std::move(iter), insert_config);
                        }
                        LOG_TRACE(fmt::format("Insert index: {} - {}", start_i, end_i));
                        return end_i - start_i;
                    };
                    SegmentOffset row_count = 0;
                    if (config.check_ts_) {
                        OneColumnIterator<float> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                        row_count = InsertHnswInner(iter);
                    } else {
                        // Not check ts in uncommitted segment when compact segment
                        OneColumnIterator<float, false> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                        row_count = InsertHnswInner(iter);
                    }
                    chunk_index_entry->SetRowCount(row_count);
                    break;
                }
                default: {
                    Status status = Status::NotSupport("Not support data type for index hnsw.");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
            }
            break;
        }
        case IndexType::kSecondary: {
            u32 seg_id = segment_entry->segment_id();
            RowID base_row_id(seg_id, 0);
            memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, base_row_id);
            u64 column_id = column_def->id();
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                const auto block_id = block_entry->block_id();
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                memory_secondary_index_->Insert(block_id, block_column_entry, buffer_mgr, 0, block_entry->row_count());
            }
            MemIndexDump();
            break;
        }
        case IndexType::kIVFFlat: { // TODO
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
    // IMPORT invokes this func at upper half at which the txn hasn't been commited yet.
    TxnTimeStamp ts = std::max(txn->BeginTS(), txn->CommitTS());
    assert(ts >= min_ts_);
    max_ts_ = ts;
}

Status SegmentIndexEntry::CreateIndexPrepare(const SegmentEntry *segment_entry, Txn *txn, bool prepare, bool check_ts) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    auto *buffer_mgr = txn->buffer_mgr();
    const IndexBase *index_base = table_index_entry_->index_base();
    const ColumnDef *column_def = table_index_entry_->column_def().get();

    PopulateEntireConfig populate_entire_config{.prepare_ = prepare, .check_ts_ = check_ts};
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
                    Status status = Status::NotSupport("Not support data type for index ivf.");
                    LOG_ERROR(status.message());
                    RecoverableError(status);
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            PopulateEntirely(segment_entry, txn, populate_entire_config);
            break;
        }
        case IndexType::kFullText: {
            PopulateEntirely(segment_entry, txn, populate_entire_config);
            break;
        }
        case IndexType::kSecondary: {
            PopulateEntirely(segment_entry, txn, populate_entire_config);
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
            auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
            GetChunkIndexEntries(chunk_index_entries);
            for (const auto &chunk_index_entry : chunk_index_entries) {
                RowID base_rowid = chunk_index_entry->base_rowid_;
                SegmentOffset offset = base_rowid.segment_offset_;
                SegmentOffset row_count = chunk_index_entry->row_count_;
                BufferHandle buffer_handle = chunk_index_entry->GetIndex();

                switch (embedding_info->Type()) {
                    case kElemFloat: {
                        AbstractHnsw<f32, SegmentOffset> abstract_hnsw(buffer_handle.GetDataMut(), index_hnsw);
                        while (true) {
                            SizeT idx = create_index_idx.fetch_add(1);
                            if (idx >= row_count) {
                                break;
                            }
                            abstract_hnsw.Build(offset + idx);
                        }
                        break;
                    }
                    default: {
                        Status status = Status::NotSupport("Not implemented");
                        LOG_ERROR(status.message());
                        RecoverableError(status);
                    }
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

void SegmentIndexEntry::CommitSegmentIndex(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock lock(rw_locker_);

    max_ts_ = commit_ts;
    if (!this->Committed()) {
        txn_id_ = txn_id;
        checkpoint_ts_ = commit_ts;
        this->Commit(commit_ts);

        for (auto &chunk_index_entry : chunk_index_entries_) {
            chunk_index_entry->Commit(commit_ts);
        }
    }
}

void SegmentIndexEntry::CommitOptimize(ChunkIndexEntry *new_chunk, const Vector<ChunkIndexEntry *> &old_chunks, TxnTimeStamp commit_ts) {
    std::unique_lock lock(rw_locker_);

    max_ts_ = commit_ts;
    new_chunk->Commit(commit_ts);
    for (auto *old_chunk : old_chunks) {
        old_chunk->DeprecateChunk(commit_ts);
    }

    // std::stringstream ss;
    // ss << "COMMIT OPTIMIZE: ";
    // ss << "Segment ID: " << segment_id_ << ". ";
    // ss << "New index chunk: " << new_chunk->chunk_id_ << ". ";
    // ss << "Old chunks: ";
    // for (auto *old_chunk : old_chunks) {
    //     ss << old_chunk->chunk_id_ << ", ";
    // }
    // LOG_INFO(ss.str());
}

bool SegmentIndexEntry::Flush(TxnTimeStamp checkpoint_ts) {
    auto index_type = table_index_entry_->index_base()->index_type_;
    if (index_type == IndexType::kFullText || index_type == IndexType::kHnsw) {
        // Fulltext index, Hnsw doesn't need to be checkpointed.
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
        buffer_obj->PickForCleanup();
    }
    for (auto &chunk_index_entry : chunk_index_entries_) {
        chunk_index_entry->Cleanup();
    }
}

void SegmentIndexEntry::PickCleanup(CleanupScanner *scanner) {
    TxnTimeStamp visible_ts = scanner->visible_ts();
    std::unique_lock w_lock(rw_locker_);
    for (auto iter = chunk_index_entries_.begin(); iter != chunk_index_entries_.end();) {
        auto &chunk_index_entry = *iter;
        if (chunk_index_entry->CheckDeprecate(visible_ts)) {
            scanner->AddEntry(std::move(chunk_index_entry));
            iter = chunk_index_entries_.erase(iter);
        } else {
            ++iter;
        }
    }
}

UniquePtr<CreateIndexParam>
SegmentIndexEntry::GetCreateIndexParam(SharedPtr<IndexBase> index_base, SizeT seg_row_count, SharedPtr<ColumnDef> column_def) {
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            return MakeUnique<CreateAnnIVFFlatParam>(index_base, column_def, seg_row_count);
        }
        case IndexType::kHnsw: {
            SizeT chunk_size = 8192; // TODO
            SizeT max_chunk_num = 1024;
            return MakeUnique<CreateHnswParam>(index_base, column_def, chunk_size, max_chunk_num);
        }
        case IndexType::kFullText: {
            return MakeUnique<CreateIndexParam>(index_base, column_def);
        }
        case IndexType::kSecondary: {
            return MakeUnique<CreateSecondaryIndexParam>(index_base, column_def, seg_row_count);
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

void SegmentIndexEntry::ReplaceChunkIndexEntries(TxnTableStore *txn_table_store,
                                                 SharedPtr<ChunkIndexEntry> merged_chunk_index_entry,
                                                 Vector<ChunkIndexEntry *> &&old_chunks) {
    TxnIndexStore *txn_index_store = txn_table_store->GetIndexStore(table_index_entry_);
    chunk_index_entries_.push_back(merged_chunk_index_entry);
    txn_index_store->optimize_data_.emplace_back(this, merged_chunk_index_entry.get(), std::move(old_chunks));
}

ChunkIndexEntry *SegmentIndexEntry::RebuildChunkIndexEntries(TxnTableStore *txn_table_store, SegmentEntry *segment_entry) {
    Txn *txn = txn_table_store->txn_;
    TxnTimeStamp begin_ts = txn->BeginTS();
    const IndexBase *index_base = table_index_entry_->index_base();
    SharedPtr<ColumnDef> column_def = table_index_entry_->column_def();

    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            BufferManager *buffer_mgr = txn->buffer_mgr();
            Vector<ChunkIndexEntry *> old_chunks;
            u32 row_count = 0;
            {
                std::shared_lock lock(rw_locker_);
                if (chunk_index_entries_.size() <= 1) { // TODO
                    return nullptr;
                }
                for (const auto &chunk_index_entry : chunk_index_entries_) {
                    if (chunk_index_entry->CheckVisible(txn)) {
                        row_count += chunk_index_entry->row_count_;
                        old_chunks.push_back(chunk_index_entry.get());
                    }
                }
            }

            auto index_hnsw = static_cast<const IndexHnsw *>(index_base);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                UnrecoverableError("HNSW supports embedding type.");
            }
            TypeInfo *type_info = column_def->type()->type_info().get();
            auto embedding_info = static_cast<EmbeddingInfo *>(type_info);

            RowID base_rowid(segment_id_, 0);
            SharedPtr<ChunkIndexEntry> merged_chunk_index_entry = CreateChunkIndexEntry(column_def, base_rowid, buffer_mgr);
            BufferHandle buffer_handle = merged_chunk_index_entry->GetIndex();

            switch (embedding_info->Type()) {
                case kElemFloat: {
                    AbstractHnsw<f32, SegmentOffset> abstract_hnsw(buffer_handle.GetDataMut(), index_hnsw);
                    OneColumnIterator<float, true /*check ts*/> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts);
                    HnswInsertConfig insert_config;
                    insert_config.optimize_ = true;
                    auto [start_i, end_i] = abstract_hnsw.InsertVecs(std::move(iter), insert_config);
                    if (end_i - start_i != row_count) {
                        UnrecoverableError("Rebuild HNSW index failed.");
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Rebuild HNSW index failed.");
                }
            }
            merged_chunk_index_entry->SetRowCount(row_count);
            ReplaceChunkIndexEntries(txn_table_store, merged_chunk_index_entry, std::move(old_chunks));
            txn_table_store->AddChunkIndexStore(table_index_entry_, merged_chunk_index_entry.get());
            return merged_chunk_index_entry.get();
        }
        case IndexType::kSecondary: {
            BufferManager *buffer_mgr = txn->buffer_mgr();
            Vector<ChunkIndexEntry *> old_chunks;
            u32 row_count = 0;
            {
                std::shared_lock lock(rw_locker_);
                if (chunk_index_entries_.size() <= 1) {
                    return nullptr;
                }
                for (const auto &chunk_index_entry : chunk_index_entries_) {
                    if (chunk_index_entry->CheckVisible(txn)) {
                        row_count += chunk_index_entry->GetRowCount();
                        old_chunks.push_back(chunk_index_entry.get());
                    }
                }
            }
            RowID base_rowid(segment_id_, 0);
            SharedPtr<ChunkIndexEntry> merged_chunk_index_entry = CreateSecondaryIndexChunkIndexEntry(base_rowid, row_count, buffer_mgr);
            BufferHandle handle = merged_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<SecondaryIndexData *>(handle.GetDataMut());
            data_ptr->InsertMergeData(old_chunks, merged_chunk_index_entry);
            ReplaceChunkIndexEntries(txn_table_store, merged_chunk_index_entry, std::move(old_chunks));
            txn_table_store->AddChunkIndexStore(table_index_entry_, merged_chunk_index_entry.get());
            return merged_chunk_index_entry.get();
        }
        default: {
            UnrecoverableError("RebuildChunkIndexEntries is not supported for this index type.");
        }
    }
    return nullptr;
}

void SegmentIndexEntry::SaveIndexFile() {
    String &index_name = *table_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    for (auto &buffer_ptr : vector_buffer_) {
        buffer_ptr->Save();
    }
    for (auto &chunk_index_entry : chunk_index_entries_) {
        chunk_index_entry->SaveIndexFile();
    }
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::CreateChunkIndexEntry(SharedPtr<ColumnDef> column_def, RowID base_rowid, BufferManager *buffer_mgr) {
    const auto &index_base = table_index_entry_->table_index_def();
    auto param = SegmentIndexEntry::GetCreateIndexParam(index_base, 0 /*segment row cnt*/, column_def);
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewChunkIndexEntry(chunk_id, this, param.get(), base_rowid, buffer_mgr);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::CreateSecondaryIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewSecondaryIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr);
}

void SegmentIndexEntry::AddChunkIndexEntry(SharedPtr<ChunkIndexEntry> chunk_index_entry) {
    std::shared_lock lock(rw_locker_);
    chunk_index_entries_.push_back(chunk_index_entry);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::AddFtChunkIndexEntry(const String &base_name, RowID base_rowid, u32 row_count) {
    std::shared_lock lock(rw_locker_);
    // row range of chunk_index_entries_ may overlop and misorder due to deprecated ones.
    SharedPtr<ChunkIndexEntry> chunk_index_entry = ChunkIndexEntry::NewFtChunkIndexEntry(this, base_name, base_rowid, row_count, buffer_manager_);
    chunk_index_entries_.push_back(chunk_index_entry);
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::AddChunkIndexEntryReplay(ChunkID chunk_id,
                                                                       TableEntry *table_entry,
                                                                       const String &base_name,
                                                                       RowID base_rowid,
                                                                       u32 row_count,
                                                                       TxnTimeStamp commit_ts,
                                                                       TxnTimeStamp deprecate_ts,
                                                                       BufferManager *buffer_mgr) {
    const auto &index_base = table_index_entry_->table_index_def();
    const auto &column_def = table_entry->GetColumnDefByName(index_base->column_name());
    auto param = SegmentIndexEntry::GetCreateIndexParam(index_base, 0 /*segment row cnt*/, column_def);

    SharedPtr<ChunkIndexEntry> chunk_index_entry =
        ChunkIndexEntry::NewReplayChunkIndexEntry(chunk_id, this, param.get(), base_name, base_rowid, row_count, commit_ts, deprecate_ts, buffer_mgr);
    chunk_index_entries_.push_back(chunk_index_entry); // replay not need lock
    if (table_index_entry_->table_index_def()->index_type_ == IndexType::kFullText) {
        u64 column_length_sum = chunk_index_entry->GetColumnLengthSum();
        UpdateFulltextColumnLenInfo(column_length_sum, row_count);
    };
    return chunk_index_entry;
}

nlohmann::json SegmentIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    if (this->deleted_) {
        UnrecoverableError("Segment Column index entry can't be deleted.");
    }

    nlohmann::json index_entry_json;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        index_entry_json["segment_id"] = this->segment_id_;
        index_entry_json["commit_ts"] = this->commit_ts_.load();
        index_entry_json["min_ts"] = this->min_ts_;
        index_entry_json["max_ts"] = this->max_ts_;
        index_entry_json["next_chunk_id"] = this->next_chunk_id_;
        index_entry_json["checkpoint_ts"] = this->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry

        for (auto &chunk_index_entry : chunk_index_entries_) {
            if (chunk_index_entry->commit_ts_ <= max_commit_ts) {
                index_entry_json["chunk_index_entries"].push_back(chunk_index_entry->Serialize());
            }
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
    SegmentID segment_id = index_entry_json["segment_id"];
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
    segment_index_entry->commit_ts_.store(index_entry_json["commit_ts"]);
    segment_index_entry->next_chunk_id_ = index_entry_json["next_chunk_id"];
    segment_index_entry->checkpoint_ts_ = index_entry_json["checkpoint_ts"]; // TODO shenyushi:: use fields in BaseEntry
    if (index_entry_json.contains("chunk_index_entries")) {
        for (const auto &chunk_index_entry_json : index_entry_json["chunk_index_entries"]) {
            SharedPtr<ChunkIndexEntry> chunk_index_entry =
                ChunkIndexEntry::Deserialize(chunk_index_entry_json, segment_index_entry.get(), create_index_param.get(), buffer_mgr);
            segment_index_entry->chunk_index_entries_.push_back(std::move(chunk_index_entry));
        }
    }

    segment_index_entry->ft_column_len_sum_ = index_entry_json["ft_column_len_sum"];
    segment_index_entry->ft_column_len_cnt_ = index_entry_json["ft_column_len_cnt"];
    return segment_index_entry;
}

} // namespace infinity
