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

#include <vector>

module table_index_entry;

import third_party;
import local_file_system;
import default_values;
import index_base;
import segment_iter;

import infinity_exception;
import index_full_text;
import catalog_delta_entry;
import base_table_ref;
import iresearch_datastore;
import create_index_info;
import base_entry;
import logger;

import index_file_worker;
import annivfflat_index_file_worker;
import hnsw_file_worker;
import secondary_index_file_worker;
import embedding_info;

namespace infinity {

TableIndexEntry::TableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                 TableIndexMeta *table_index_meta,
                                 SharedPtr<String> index_dir,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts,
                                 bool is_replay)
    : BaseEntry(EntryType::kTableIndex), table_index_meta_(table_index_meta), index_base_(index_base), index_dir_(std::move(index_dir)),
      byte_slice_pool_(), buffer_pool_(), thread_pool_(4) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

TableIndexEntry::TableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTableIndex), table_index_meta_(table_index_meta), byte_slice_pool_(), buffer_pool_(), thread_pool_(4) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewTableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                                               TableIndexMeta *table_index_meta,
                                                               Txn *txn,
                                                               TransactionID txn_id,
                                                               TxnTimeStamp begin_ts,
                                                               bool is_replay,
                                                               String replay_table_index_dir) {

    if (is_replay) {
        auto table_index_entry =
            MakeShared<TableIndexEntry>(index_base, table_index_meta, MakeShared<String>(replay_table_index_dir), txn_id, begin_ts);
        return table_index_entry;
    } else {
        SharedPtr<String> index_dir = DetermineIndexDir(*table_index_meta->GetTableEntry()->TableEntryDir(), *index_base->index_name_);
        auto table_index_entry = MakeShared<TableIndexEntry>(index_base, table_index_meta, index_dir, txn_id, begin_ts);

        TableIndexEntry *table_index_entry_ptr = table_index_entry.get();

        {
            auto operation = MakeUnique<AddTableIndexEntryOp>(table_index_entry);
            txn->AddCatalogDeltaOperation(std::move(operation));
        }

        // Get column info
        if (index_base->column_names_.size() != 1) {
            RecoverableError(Status::SyntaxError("Currently, composite index doesn't supported."));
        }
        if (index_base->index_type_ == IndexType::kFullText) {
            SharedPtr<IndexFullText> index_fulltext = std::static_pointer_cast<IndexFullText>(index_base);
            if (index_fulltext->homebrewed_) {
                // TODO yzc: remove table_index_entry->fulltext_index_entry_
            } else {
                table_index_entry->fulltext_index_entry_ =
                    FulltextIndexEntry::NewFulltextIndexEntry(table_index_entry_ptr, txn, txn_id, table_index_entry->index_dir_, begin_ts);
            }
        }

        return table_index_entry;
    }
}

bool TableIndexEntry::IsFulltextIndexHomebrewed() const {
    bool homebrewed = false;
    if (index_base_->index_type_ == IndexType::kFullText) {
        SharedPtr<IndexFullText> index_fulltext = std::static_pointer_cast<IndexFullText>(index_base_);
        homebrewed = index_fulltext->homebrewed_;
    }
    return homebrewed;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                                     const SharedPtr<IndexBase> &index_base,
                                                                     const SharedPtr<String> &index_dir,
                                                                     TransactionID txn_id,
                                                                     TxnTimeStamp begin_ts,
                                                                     TxnTimeStamp commit_ts,
                                                                     bool is_delete) noexcept {
    auto table_index_entry = MakeShared<TableIndexEntry>(index_base, table_index_meta, index_dir, txn_id, begin_ts);
    table_index_entry->commit_ts_.store(commit_ts);
    table_index_entry->deleted_ = is_delete;
    return table_index_entry;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewDropTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto dropped_index_entry = MakeShared<TableIndexEntry>(nullptr, table_index_meta, nullptr, txn_id, begin_ts);
    return dropped_index_entry;
}

SharedPtr<TableIndexEntry>
TableIndexEntry::NewDropReplayTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts) {
    return MakeShared<TableIndexEntry>(nullptr, table_index_meta, nullptr, txn_id, begin_ts);
}

// For segment_index_entry
void TableIndexEntry::CommitCreateIndex(TxnIndexStore *txn_index_store, TxnTimeStamp commit_ts, bool is_replay) {
    {
        std::unique_lock w_lock(rw_locker_);
        for (const auto &[segment_id, segment_index_entry] : txn_index_store->index_entry_map_) {
            if (!is_replay) {
                // Save index file.
                segment_index_entry->SaveIndexFile();
            }
            segment_index_entry->Commit(commit_ts);
        }
        if (txn_index_store->fulltext_index_entry_ != nullptr) {
            txn_index_store->fulltext_index_entry_->Commit(commit_ts);
        }
    }
}

// For fulltext_index_entry
void TableIndexEntry::RollbackCreateIndex(TxnIndexStore *txn_index_store) {
    {
        std::unique_lock w_lock(rw_locker_);
        for (const auto &[segment_id, segment_index_entry] : txn_index_store->index_entry_map_) {
            segment_index_entry->Cleanup();
            if (index_by_segment_.erase(segment_id) == 0) {
                UnrecoverableError("Failed to erase segment index entry");
            }
        }
        if (txn_index_store->fulltext_index_entry_ != nullptr) {
            txn_index_store->fulltext_index_entry_->Cleanup();
            fulltext_index_entry_.reset();
        }
    }
}

nlohmann::json TableIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json;

    Vector<SegmentIndexEntry *> segment_index_entry_candidates;
    FulltextIndexEntry *fulltext_index_entry_candidate_{};
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json["txn_id"] = this->txn_id_.load();
        json["begin_ts"] = this->begin_ts_;
        json["commit_ts"] = this->commit_ts_.load();
        json["deleted"] = this->deleted_;
        if (this->deleted_) {
            return json;
        }

        json["index_dir"] = *this->index_dir_;
        json["index_base"] = this->index_base_->Serialize();

        for (const auto &[segment_id, index_entry] : this->index_by_segment_) {
            segment_index_entry_candidates.emplace_back((SegmentIndexEntry *)index_entry.get());
        }

        fulltext_index_entry_candidate_ = this->fulltext_index_entry_.get();
    }

    for (const auto &segment_index_entry : segment_index_entry_candidates) {
        json["segment_indexes"].emplace_back(segment_index_entry->Serialize());
    }

    if (fulltext_index_entry_candidate_ != nullptr) {
        json["fulltext_index_entry"] = fulltext_index_entry_candidate_->Serialize(max_commit_ts);
    }

    return json;
}

SharedPtr<TableIndexEntry> TableIndexEntry::Deserialize(const nlohmann::json &index_def_entry_json,
                                                        TableIndexMeta *table_index_meta,
                                                        BufferManager *buffer_mgr,
                                                        TableEntry *table_entry) {
    TransactionID txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    bool deleted = index_def_entry_json["deleted"];

    if (deleted) {
        auto table_index_entry = NewDropReplayTableIndexEntry(table_index_meta, txn_id, begin_ts);
        table_index_entry->deleted_ = true;
        table_index_entry->commit_ts_.store(commit_ts);
        table_index_entry->begin_ts_ = begin_ts;
        return table_index_entry;
    }

    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);
    auto index_base = IndexBase::Deserialize(index_def_entry_json["index_base"]);

    SharedPtr<TableIndexEntry> table_index_entry = MakeShared<TableIndexEntry>(index_base, table_index_meta, index_dir, txn_id, begin_ts, true);
    table_index_entry->commit_ts_.store(commit_ts);
    table_index_entry->begin_ts_ = begin_ts;

    if (index_def_entry_json.contains("segment_indexes")) {
        for (const auto &segment_index_entry_json : index_def_entry_json["segment_indexes"]) {
            SharedPtr<SegmentIndexEntry> segment_index_entry =
                SegmentIndexEntry::Deserialize(segment_index_entry_json, table_index_entry.get(), buffer_mgr, table_entry);
            table_index_entry->index_by_segment_.emplace(segment_index_entry->segment_id(), std::move(segment_index_entry));
        }
    }

    if (index_def_entry_json.contains("fulltext_index_entry")) {
        table_index_entry->fulltext_index_entry_ =
            FulltextIndexEntry::Deserialize(index_def_entry_json["fulltext_index_entry"], table_index_entry.get(), buffer_mgr);
    }
    return table_index_entry;
}

Tuple<FulltextIndexEntry *, Vector<SegmentIndexEntry *>, Status>
TableIndexEntry::CreateIndexPrepare(TableEntry *table_entry, BlockIndex *block_index, Txn *txn, bool prepare, bool is_replay, bool check_ts) {
    FulltextIndexEntry *fulltext_index_entry = this->fulltext_index_entry_.get();
    if (fulltext_index_entry != nullptr && !IsFulltextIndexHomebrewed()) {
        auto *buffer_mgr = txn->buffer_manager();
        for (const auto *segment_entry : block_index->segments_) {
            fulltext_index_entry->irs_index_->BatchInsert(table_entry, index_base_.get(), segment_entry, buffer_mgr);
        }
        fulltext_index_entry->irs_index_->Commit();
        fulltext_index_entry->irs_index_->StopSchedule();
        return {fulltext_index_entry, Vector<SegmentIndexEntry *>{}, Status::OK()};
    }
    const String &column_name = this->index_base()->column_name();
    const auto *column_def = table_entry->GetColumnDefByName(column_name);
    Vector<SegmentIndexEntry *> segment_index_entries;
    for (const auto *segment_entry : block_index->segments_) {
        auto create_index_param = SegmentIndexEntry::GetCreateIndexParam(index_base_.get(), segment_entry->row_count(), column_def);
        SegmentID segment_id = segment_entry->segment_id();
        SharedPtr<SegmentIndexEntry> segment_index_entry = SegmentIndexEntry::NewIndexEntry(this, segment_id, txn, create_index_param.get());
        if (!is_replay) {
            segment_index_entry->CreateIndexPrepare(index_base_.get(), column_def, segment_entry, txn, prepare, check_ts);
        }
        index_by_segment_.emplace(segment_id, segment_index_entry);
        segment_index_entries.push_back(segment_index_entry.get());
    }
    return {nullptr, segment_index_entries, Status::OK()};
}

Status TableIndexEntry::CreateIndexDo(const TableEntry *table_entry, HashMap<SegmentID, atomic_u64> &create_index_idxes) {
    if (this->index_base_->column_names_.size() != 1) {
        // TODO
        UnrecoverableError("Not implemented");
    }
    const String &column_name = this->index_base_->column_name();
    const auto *column_def = table_entry->GetColumnDefByName(column_name);
    for (auto &[segment_id, segment_index_entry] : index_by_segment_) {
        atomic_u64 &create_index_idx = create_index_idxes.at(segment_id);
        auto status = segment_index_entry->CreateIndexDo(index_base_.get(), column_def, create_index_idx);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Vector<UniquePtr<IndexFileWorker>> TableIndexEntry::CreateFileWorker(CreateIndexParam *param, u32 segment_id) {
    Vector<UniquePtr<IndexFileWorker>> vector_file_worker;
    // reference file_worker will be invalidated when vector_file_worker is resized
    const auto *index_base = param->index_base_;
    const auto *column_def = param->column_def_;
    if (index_base->index_type_ == IndexType::kFullText) {
        // fulltext doesn't use BufferManager
        return vector_file_worker;
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
                    file_worker = MakeUnique<AnnIVFFlatIndexFileWorker<f32>>(this->index_dir(),
                                                                             file_name,
                                                                             index_base,
                                                                             column_def,
                                                                             create_annivfflat_param->row_count_);
                    break;
                }
                default: {
                    UnrecoverableError("Create IVF Flat index: Unsupported element type.");
                }
            }
            break;
        }
        case IndexType::kHnsw: {
            auto create_hnsw_param = static_cast<CreateHnswParam *>(param);
            file_worker = MakeUnique<HnswFileWorker>(this->index_dir(), file_name, index_base, column_def, create_hnsw_param->max_element_);
            break;
        }
        case IndexType::kFullText: {
            // fulltext doesn't use BufferManager
            break;
        }
        case IndexType::kSecondary: {
            auto create_secondary_param = static_cast<CreateSecondaryIndexParam *>(param);
            auto const row_count = create_secondary_param->row_count_;
            auto const part_capacity = create_secondary_param->part_capacity_;
            // now we can only use row_count to calculate the part_num
            // because the actual_row_count will reduce when we delete rows
            // consider the timestamp, actual_row_count may be less than, equal to or greater than rows we can actually read
            u32 part_num = (row_count + part_capacity - 1) / part_capacity;
            vector_file_worker.resize(part_num + 1);
            // cannot use invalid file_worker
            vector_file_worker[0] =
                MakeUnique<SecondaryIndexFileWorker>(this->index_dir(), file_name, index_base, column_def, 0, row_count, part_capacity);
            for (u32 i = 1; i <= part_num; ++i) {
                auto part_file_name = MakeShared<String>(fmt::format("{}_part{}", *file_name, i));
                vector_file_worker[i] =
                    MakeUnique<SecondaryIndexFileWorker>(this->index_dir(), part_file_name, index_base, column_def, i, row_count, part_capacity);
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

UniquePtr<CreateIndexParam> TableIndexEntry::GetCreateIndexParam(const IndexBase *index_base, SizeT seg_row_count, const ColumnDef *column_def) {
    switch (index_base->index_type_) {
        case IndexType::kIVFFlat: {
            return MakeUnique<CreateAnnIVFFlatParam>(index_base, column_def, seg_row_count);
        }
        case IndexType::kHnsw: {
            SizeT max_element = seg_row_count;
            return MakeUnique<CreateHnswParam>(index_base, column_def, max_element);
        }
        case IndexType::kFullText: {
            return MakeUnique<CreateFullTextParam>(index_base, column_def);
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

void TableIndexEntry::Cleanup() {
    if (this->deleted_) {
        return;
    }
    for (auto &[segment_id, segment_index_entry] : index_by_segment_) {
        segment_index_entry->Cleanup();
    }
    if (this->fulltext_index_entry_.get() != nullptr) {
        fulltext_index_entry_->Cleanup();
    }

    LOG_INFO(fmt::format("Cleanup dir: {}", *index_dir_));
    LocalFileSystem fs;
    fs.DeleteDirectory(*index_dir_); // FIXME(sys): delete full text index by whole directory tmply
}

void TableIndexEntry::PickCleanup(CleanupScanner *scanner) {}

void TableIndexEntry::PickCleanupBySegments(const Vector<SegmentID> &sorted_segment_ids, CleanupScanner *scanner) {
    for (auto iter = index_by_segment_.begin(); iter != index_by_segment_.end();) {
        auto &[segment_id, segment_index_entry] = *iter;
        if (std::binary_search(sorted_segment_ids.begin(), sorted_segment_ids.end(), segment_id)) {
            scanner->AddEntry(std::move(segment_index_entry));
            iter = index_by_segment_.erase(iter);
        } else {
            ++iter;
        }
    }
}

} // namespace infinity
