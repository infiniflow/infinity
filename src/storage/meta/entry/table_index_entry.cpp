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
#include <vector>

module table_index_entry;

import third_party;
import virtual_store;
import default_values;
import index_base;
import segment_iter;
import hnsw_util;
import infinity_exception;
import index_full_text;
import catalog_delta_entry;
import base_table_ref;
import create_index_info;
import base_entry;
import logger;
import index_hnsw;
import index_file_worker;
import embedding_info;
import block_entry;
import segment_entry;
import table_entry;
import infinity_context;
import block_index;

namespace infinity {

SharedPtr<String> TableIndexEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    auto abs_parent_dir = Path(InfinityContext::instance().config()->DataDir()) / parent_dir;
    SharedPtr<String> temp_dir = DetermineRandomString(abs_parent_dir, fmt::format("index_{}", index_name));
    return MakeShared<String>(Path(parent_dir) / *temp_dir);
}

Vector<std::string_view> TableIndexEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid table index entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = TableEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String TableIndexEntry::EncodeIndex(const String &index_name, TableIndexMeta *index_meta) {
    if (index_meta == nullptr) {
        return ""; // unit test
    }
    return fmt::format("{}#{}", index_meta->GetTableEntry()->encode(), index_name);
}

TableIndexEntry::TableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                 bool is_delete,
                                 TableIndexMeta *table_index_meta,
                                 const SharedPtr<String> &index_entry_dir,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTableIndex, is_delete, TableIndexEntry::EncodeIndex(*index_base->index_name_, table_index_meta)),
      table_index_meta_(table_index_meta), index_base_(std::move(index_base)), index_dir_(index_entry_dir) {
    if (!is_delete) {
        assert(index_base.get() != nullptr);
        const String &column_name = index_base->column_name();
        column_def_ = table_index_meta->GetTableEntry()->GetColumnDefByName(column_name);
        if (column_def_.get() == nullptr) {
            UnrecoverableError(fmt::format("Column {} not found in table {}.", column_name, *table_index_meta->GetTableEntry()->GetTableName()));
        }
    }
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

TableIndexEntry::TableIndexEntry(const TableIndexEntry &other)
    : BaseEntry(other), table_index_meta_(other.table_index_meta_), index_base_(other.index_base_), index_dir_(other.index_dir_),
      column_def_(other.column_def_) {}

UniquePtr<TableIndexEntry> TableIndexEntry::Clone(TableIndexMeta *table_index_meta) const {
    auto ret = UniquePtr<TableIndexEntry>(new TableIndexEntry(*this));
    ret->table_index_meta_ = table_index_meta;
    std::shared_lock lock(rw_locker_);
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        ret->index_by_segment_.emplace(segment_id, segment_index_entry->Clone(ret.get()));
    }
    return ret;
}

SharedPtr<TableIndexEntry> TableIndexEntry::NewTableIndexEntry(const SharedPtr<IndexBase> &index_base,
                                                               bool is_delete,
                                                               const SharedPtr<String> &table_entry_dir,
                                                               TableIndexMeta *table_index_meta,
                                                               TransactionID txn_id,
                                                               TxnTimeStamp begin_ts) {
    if (is_delete) {
        return MakeShared<TableIndexEntry>(index_base, is_delete, table_index_meta, nullptr, txn_id, begin_ts);
    }
    SharedPtr<String> index_dir = DetermineIndexDir(*table_index_meta->GetTableEntry()->TableEntryDir(), *index_base->index_name_);
    auto table_index_entry = MakeShared<TableIndexEntry>(index_base, is_delete, table_index_meta, index_dir, txn_id, begin_ts);

    // Get column info
    if (index_base->column_names_.size() != 1) {
        Status status = Status::SyntaxError("Currently, composite index doesn't supported.");
        RecoverableError(status);
    }
    return table_index_entry;
}

SharedPtr<TableIndexEntry> TableIndexEntry::ReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                                  bool is_delete,
                                                                  const SharedPtr<IndexBase> &index_base,
                                                                  const SharedPtr<String> &index_entry_dir,
                                                                  TransactionID txn_id,
                                                                  TxnTimeStamp begin_ts,
                                                                  TxnTimeStamp commit_ts) noexcept {
    auto table_index_entry = MakeShared<TableIndexEntry>(index_base, is_delete, table_index_meta, index_entry_dir, txn_id, begin_ts);
    table_index_entry->commit_ts_.store(commit_ts);
    return table_index_entry;
}

Map<SegmentID, SharedPtr<SegmentIndexEntry>> TableIndexEntry::GetIndexBySegmentSnapshot(const TableEntry *table_entry, Txn *txn) {
    std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment_snapshot;
    for (const auto &[segment_id, segment_index_entry] : this->index_by_segment_) {
        auto segment_entry = table_entry->GetSegmentByID(segment_id, txn);
        if (segment_entry.get() == nullptr) {
            continue;
        }
        index_by_segment_snapshot.emplace(segment_id, segment_index_entry);
    }
    return index_by_segment_snapshot;
}

String TableIndexEntry::GetPathNameTail() const {
    SizeT delimiter_i = index_dir_->rfind('/');
    if (delimiter_i == String::npos) {
        return *index_dir_;
    }
    return index_dir_->substr(delimiter_i + 1);
}

SharedPtr<SegmentIndexEntry> TableIndexEntry::GetSegment(SegmentID segment_id, Txn *txn) {
    std::unique_lock w_lock(rw_locker_);
    auto iter = index_by_segment_.find(segment_id);
    if (iter == index_by_segment_.end() || !iter->second->CheckDeprecate(txn->BeginTS())) {
        return nullptr;
    }
    return iter->second;
}

bool TableIndexEntry::GetOrCreateSegment(SegmentID segment_id, Txn *txn, SharedPtr<SegmentIndexEntry> &segment_index_entry) {
    bool created = false;
    std::unique_lock w_lock(rw_locker_);
    auto iter = index_by_segment_.find(segment_id);
    if (iter == index_by_segment_.end()) {
        segment_index_entry = SegmentIndexEntry::NewIndexEntry(this, segment_id, txn);
        index_by_segment_.emplace(segment_id, segment_index_entry);
        created = true;
        LOG_TRACE(fmt::format("{} Create segment index entry for segment {}", this->encode(), segment_id));
    } else {
        segment_index_entry = iter->second;
    }
    return created;
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
            segment_index_entry->CommitIndex(commit_ts);
        }
        if (!Committed()) {
            commit_ts_.store(commit_ts);
        }
    }
    if (index_base()->index_type_ == IndexType::kFullText) {
        UpdateFulltextSegmentTs(commit_ts);
    }
}

// void TableIndexEntry::RollbackPopulateIndex(TxnIndexStore *txn_index_store, Txn *txn) {
//     std::unique_lock w_lock(rw_locker_);
//     for (auto *chunk_index_entry : txn_index_store->chunk_index_entries_) {
//         chunk_index_entry->Cleanup();
//         SegmentIndexEntry *segment_index_entry = chunk_index_entry->segment_index_entry_;
//         segment_index_entry->RemoveChunkIndexEntry(chunk_index_entry);
//     }
//     for (const auto &[segment_id, segment_index_entry] : txn_index_store->index_entry_map_) {
//         if (segment_index_entry->begin_ts_ != txn->BeginTS()) {
//             continue;
//         }
//         segment_index_entry->Cleanup();
//         if (index_by_segment_.erase(segment_id) == 0) {
//             UnrecoverableError("Failed to erase segment index entry");
//         }
//     }
// }

void TableIndexEntry::CommitCompact([[maybe_unused]] TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store) {
    std::unique_lock w_lock(rw_locker_);
    for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {
        auto *segment_entry = segment_store.segment_entry_;

        auto iter = index_by_segment_.find(segment_entry->segment_id());
        if (iter == index_by_segment_.end()) {
            continue;
        }
        [[maybe_unused]] auto *segment_index_entry = iter->second.get();

        for (auto *old_segment : old_segments) {
            auto iter = index_by_segment_.find(old_segment->segment_id());
            if (iter == index_by_segment_.end()) {
                continue;
            }
            auto *old_segment_index_entry = iter->second.get();
            old_segment_index_entry->SetDeprecated(commit_ts);
        }
    }
}

nlohmann::json TableIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json;

    Vector<SharedPtr<SegmentIndexEntry>> segment_index_entry_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json["txn_id"] = this->txn_id_;
        json["begin_ts"] = this->begin_ts_;
        json["commit_ts"] = this->commit_ts_.load();
        json["deleted"] = this->deleted_;
        if (this->deleted_) {
            return json;
        }

        json["index_dir"] = *this->index_dir_;
        json["index_base"] = this->index_base_->Serialize();

        for (const auto &[segment_id, index_entry] : this->index_by_segment_) {
            if (!index_entry->CheckDeprecate(max_commit_ts)) {
                segment_index_entry_candidates.push_back(index_entry);
            }
        }
    }

    for (const auto &segment_index_entry : segment_index_entry_candidates) {
        json["segment_indexes"].emplace_back(segment_index_entry->Serialize(max_commit_ts));
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
        auto index_name = table_index_meta->index_name();
        auto table_index_entry =
            ReplayTableIndexEntry(table_index_meta, true, MakeShared<IndexBase>(index_name), nullptr, txn_id, begin_ts, commit_ts);
        table_index_entry->commit_ts_.store(commit_ts);
        table_index_entry->begin_ts_ = begin_ts;
        return table_index_entry;
    }

    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);
    auto index_base = IndexBase::Deserialize(index_def_entry_json["index_base"]);

    SharedPtr<TableIndexEntry> table_index_entry = ReplayTableIndexEntry(table_index_meta, false, index_base, index_dir, txn_id, begin_ts, commit_ts);
    table_index_entry->commit_ts_.store(commit_ts);
    table_index_entry->begin_ts_ = begin_ts;

    if (index_def_entry_json.contains("segment_indexes")) {
        for (const auto &segment_index_entry_json : index_def_entry_json["segment_indexes"]) {
            SharedPtr<SegmentIndexEntry> segment_index_entry =
                SegmentIndexEntry::Deserialize(segment_index_entry_json, table_index_entry.get(), buffer_mgr, table_entry);
            table_index_entry->index_by_segment_.emplace(segment_index_entry->segment_id(), std::move(segment_index_entry));
        }
    }
    return table_index_entry;
}

Vector<BaseMemIndex *> TableIndexEntry::GetMemIndex() const {
    Vector<BaseMemIndex *> res;
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        auto *mem_idx = segment_index_entry->GetMemIndex();
        if (mem_idx) {
            res.push_back(mem_idx);
        }
    }
    return res;
}

void TableIndexEntry::MemIndexCommit() {
    Vector<SegmentIndexEntry *> segment_index_entries;
    {
        std::shared_lock r_lock(rw_locker_);
        for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
            segment_index_entries.push_back(segment_index_entry.get());
        }
    }
    for (auto *segment_index_entry : segment_index_entries) {
        segment_index_entry->MemIndexCommit();
    }
}

// this function is called when memory index used up quota, and need to dump.
Vector<SharedPtr<ChunkIndexEntry>> TableIndexEntry::MemIndexDump(Txn *txn, TxnTableStore *txn_table_store, bool spill, SizeT *dump_size) {
    Vector<SegmentIndexEntry *> segment_index_entries;
    {
        std::shared_lock r_lock(rw_locker_);
        for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
            segment_index_entries.push_back(segment_index_entry.get());
        }
    }
    Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
    for (auto *segment_index_entry : segment_index_entries) {
        auto chunk_index_entry = segment_index_entry->MemIndexDump(spill, dump_size);
        txn_table_store->AddChunkIndexStore(this, chunk_index_entry.get());
        segment_index_entry->AddWalIndexDump(chunk_index_entry.get(), txn);
    }
    return chunk_index_entries;
}

SharedPtr<SegmentIndexEntry> TableIndexEntry::PopulateEntirely(SegmentEntry *segment_entry, Txn *txn, const PopulateEntireConfig &config) {
    switch (index_base_->index_type_) {
        case IndexType::kHnsw:
        case IndexType::kIVF:
        case IndexType::kEMVB:
        case IndexType::kFullText:
        case IndexType::kSecondary:
        case IndexType::kBMP: {
            break;
        }
        default: {
            return nullptr;
        }
    }
    u32 segment_id = segment_entry->segment_id();
    SharedPtr<SegmentIndexEntry> segment_index_entry = SegmentIndexEntry::NewIndexEntry(this, segment_id, txn);
    segment_index_entry->PopulateEntirely(segment_entry, txn, config);
    std::unique_lock w_lock(rw_locker_);
    index_by_segment_.emplace(segment_id, segment_index_entry);

    TxnStore *txn_store = txn->txn_store();
    TxnTableStore *txn_table_store = txn_store->GetTxnTableStore(table_index_meta()->GetTableEntry());
    txn_table_store->AddSegmentIndexesStore(this, {segment_index_entry.get()});

    Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries = segment_index_entry->GetChunks();
    for (auto &chunk_index_entry : chunk_index_entries) {
        txn_table_store->AddChunkIndexStore(this, chunk_index_entry.get());
    }

    return segment_index_entry;
}

Tuple<Vector<SegmentIndexEntry *>, Status>
TableIndexEntry::CreateIndexPrepare(BaseTableRef *table_ref, Txn *txn, bool prepare, bool is_replay, bool check_ts) {
    //    TableEntry *table_entry = table_index_meta()->GetTableEntry();
    //    TxnStore *txn_store = txn->txn_store();
    //    TxnTableStore *txn_table_store = txn_store->GetTxnTableStore(table_entry);
    //
    //    auto &block_index = table_ref->block_index_;
    //    if (table_ref->index_index_.get() == nullptr) {
    //        // Note1:
    //        // If same table_ref is called in multiple threads, table_ref->index_index_ should be initialized in advance
    //        table_ref->index_index_ = MakeShared<IndexIndex>();
    //    }
    Vector<SegmentIndexEntry *> segment_index_entries;
    //    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
    //        auto *segment_entry = segment_info.segment_entry_;
    //        SharedPtr<SegmentIndexEntry> segment_index_entry = SegmentIndexEntry::NewIndexEntry(this, segment_id, txn);
    //        if (!is_replay) {
    //            segment_index_entry->CreateIndexPrepare(segment_entry, txn, prepare, check_ts);
    //        }
    //        std::unique_lock w_lock(rw_locker_);
    //        index_by_segment_.emplace(segment_id, segment_index_entry);
    //        segment_index_entries.push_back(segment_index_entry.get());
    //        txn_table_store->AddSegmentIndexesStore(this, {segment_index_entry.get()});
    //        Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries = segment_index_entry->GetChunks();
    //        for (auto &chunk_index_entry : chunk_index_entries) {
    //            txn_table_store->AddChunkIndexStore(this, chunk_index_entry.get());
    //        }
    //    }
    return {segment_index_entries, Status::OK()};
}

Status TableIndexEntry::CreateIndexDo(BaseTableRef *table_ref, HashMap<SegmentID, atomic_u64> &create_index_idxes, Txn *txn) {
    //    if (this->index_base_->column_names_.size() != 1) {
    //        // TODO
    //        Status status = Status::NotSupport("Not implemented");
    //        RecoverableError(status);
    //    }
    //    auto &index_index = table_ref->index_index_;
    //    auto iter = index_index->index_snapshots_.find(*index_base_->index_name_);
    //    if (iter == index_index->index_snapshots_.end()) {
    //        return Status::OK();
    //    }
    //    auto &segment_index_snapshots = iter->second;
    //    for (auto &[segment_id, segment_index_entry] : segment_index_snapshots->segment_index_entries_) {
    //        atomic_u64 &create_index_idx = create_index_idxes.at(segment_id);
    //        auto status = segment_index_entry->CreateIndexDo(create_index_idx);
    //        if (!status.ok()) {
    //            return status;
    //        }
    //    }
    return Status::OK();
}

Status TableIndexEntry::CreateIndexDo(const Map<SegmentID, SegmentIndexEntry *> &segment_index_entries,
                                      HashMap<SegmentID, atomic_u64> &create_index_idxes,
                                      Txn *txn) {
    if (this->index_base_->column_names_.size() != 1) {
        // TODO
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
    for (auto &[segment_id, segment_index_entry] : segment_index_entries) {
        atomic_u64 &create_index_idx = create_index_idxes.at(segment_id);
        auto status = segment_index_entry->CreateIndexDo(create_index_idx);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

void TableIndexEntry::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    if (this->deleted_) {
        return;
    }
    std::unique_lock w_lock(rw_locker_);
    for (auto &[segment_id, segment_index_entry] : index_by_segment_) {
        segment_index_entry->Cleanup(info_tracer, dropped);
    }

    if (dropped) {
        LOG_DEBUG(fmt::format("Cleaning up dir: {}", *index_dir_));

        String absolute_index_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir_);
        if (!VirtualStore::Exists(absolute_index_dir)) {
            return;
        }
        VirtualStore::RemoveDirectory(absolute_index_dir);
        LOG_DEBUG(fmt::format("Cleaned dir: {}", absolute_index_dir));
        if (info_tracer != nullptr) {
            info_tracer->AddCleanupInfo(std::move(absolute_index_dir));
        }
    }
}

Vector<String> TableIndexEntry::GetFilePath(Txn *txn) const {
    std::shared_lock lock(rw_locker_);
    Vector<String> res;
    res.reserve(index_by_segment_.size());
    for (const auto &index_pair : index_by_segment_) {
        const SegmentIndexEntry *segment_index_entry = index_pair.second.get();
        Vector<String> segment_files = segment_index_entry->GetFilePath(txn);
        res.insert(res.end(), segment_files.begin(), segment_files.end());
    }
    return res;
}

void TableIndexEntry::PickCleanup(CleanupScanner *scanner) {
    TxnTimeStamp visible_ts = scanner->visible_ts();
    Vector<SegmentID> segment_ids;
    {
        std::shared_lock r_lock(rw_locker_);
        for (auto &[segment_id, segment_index_entry] : index_by_segment_) {
            if (segment_index_entry->CheckDeprecate(visible_ts)) {
                segment_ids.push_back(segment_id);
            } else {
                segment_index_entry->PickCleanup(scanner);
            }
        }
    }
    if (!segment_ids.empty()) {
        std::unique_lock w_lock(rw_locker_);
        for (auto segment_id : segment_ids) {
            auto iter = index_by_segment_.find(segment_id);
            scanner->AddEntry(std::move(iter->second));
            index_by_segment_.erase(iter);
        }
    }
}

void TableIndexEntry::PickCleanupBySegments(const Vector<SegmentID> &sorted_segment_ids, CleanupScanner *scanner) {
    std::unique_lock w_lock(rw_locker_);
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

void TableIndexEntry::UpdateFulltextSegmentTs(TxnTimeStamp ts) {
    table_index_meta()->GetTableEntry()->UpdateFullTextSegmentTs(ts, segment_update_ts_mutex_, segment_update_ts_);
}

void TableIndexEntry::UpdateEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    commit_ts_.store(commit_ts);
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

void TableIndexEntry::OptIndex(TxnTableStore *txn_table_store, const Vector<UniquePtr<InitParameter>> &opt_params, bool replay) {
    switch (index_base_->index_type_) {
        case IndexType::kBMP: {
            if (replay) {
                break;
            }
            std::unique_lock w_lock(rw_locker_);
            for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
                segment_index_entry->OptIndex(index_base_, txn_table_store, opt_params, false /*replay*/);
            }
            break;
        }
        case IndexType::kHnsw: {
            std::unique_lock w_lock(rw_locker_);

            auto params = HnswUtil::ParseOptimizeOptions(opt_params);
            if (!params) {
                break;
            }
            if (params->compress_to_lvq) {
                auto *hnsw_index = static_cast<IndexHnsw *>(index_base_.get());
                if (hnsw_index->encode_type_ != HnswEncodeType::kPlain) {
                    LOG_WARN("Not implemented");
                    break;
                }
                auto new_index_hnsw = MakeShared<IndexHnsw>(*hnsw_index);
                // IndexHnsw old_index_hnsw = *hnsw_index;
                new_index_hnsw->encode_type_ = HnswEncodeType::kLVQ;
                if (new_index_hnsw->build_type_ == HnswBuildType::kLSG) {
                    new_index_hnsw->build_type_ = HnswBuildType::kPlain;
                }

                if (!replay) {
                    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
                        segment_index_entry->OptIndex(static_pointer_cast<IndexBase>(new_index_hnsw), txn_table_store, opt_params, false /*replay*/);
                    }
                }
                index_base_ = static_pointer_cast<IndexBase>(new_index_hnsw);
                txn_table_store->AddIndexStore(this);
            }
            if (params->lvq_avg) {
                for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
                    segment_index_entry->OptIndex(index_base_, txn_table_store, opt_params, false /*replay*/);
                }
            }
            break;
        }
        default: {
            LOG_WARN("Not implemented");
        }
    }
}

bool TableIndexEntry::CheckIfIndexColumn(ColumnID column_id) const { return static_cast<ColumnID>(column_def_->id()) == column_id; }

SharedPtr<TableIndexSnapshotInfo> TableIndexEntry::GetSnapshotInfo(Txn *txn_ptr) const {
    std::shared_lock lock(rw_locker_);
    auto snapshot_info = MakeShared<TableIndexSnapshotInfo>();
    snapshot_info->index_base_ = index_base_;
    snapshot_info->index_dir_ = index_dir_;
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        snapshot_info->index_by_segment_.emplace(segment_id, segment_index_entry->GetSnapshotInfo(txn_ptr));
    }
    return snapshot_info;
}

SharedPtr<TableIndexInfo> TableIndexEntry::GetTableIndexInfo(Txn *txn_ptr) {
    std::shared_lock lock(rw_locker_);
    SharedPtr<TableIndexInfo> table_index_info = MakeShared<TableIndexInfo>();
    table_index_info->index_name_ = index_base_->index_name_;
    table_index_info->index_entry_dir_ = index_dir_;
    table_index_info->segment_index_count_ = index_by_segment_.size();

    table_index_info->index_comment_ = MakeShared<String>(*index_base_->index_comment_);
    table_index_info->index_type_ = MakeShared<String>(IndexInfo::IndexTypeToString(index_base_->index_type_));
    table_index_info->index_other_params_ = MakeShared<String>(index_base_->BuildOtherParamsString());
    table_index_info->index_column_names_ = MakeShared<String>(column_def_->name_);
    table_index_info->index_column_ids_ = MakeShared<String>(std::to_string(column_def_->id_));
    return table_index_info;
}

} // namespace infinity
