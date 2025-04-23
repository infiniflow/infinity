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

#include <ranges>
#include <string>
#include <vector>

module new_txn_store;

import stl;
import third_party;

import status;
import infinity_exception;
import data_block;
import logger;
import data_access_state;
import new_txn;
import default_values;
import table_entry;
import catalog;
import catalog_delta_entry;
import internal_types;
import data_type;
import background_process;
import bg_task;
import compact_statement;
import build_fast_rough_filter_task;
import create_index_info;
import persistence_manager;
import infinity_context;
import persist_result_handler;
import wal_entry;
import kv_store;

namespace infinity {

NewTxnSegmentStore NewTxnSegmentStore::AddSegmentStore(SegmentEntry *segment_entry) {
    NewTxnSegmentStore txn_segment_store(segment_entry);
    for (auto &block_entry : segment_entry->block_entries()) {
        txn_segment_store.block_entries_.emplace(block_entry->block_id(), block_entry.get());
    }
    return txn_segment_store;
}

NewTxnSegmentStore::NewTxnSegmentStore(SegmentEntry *segment_entry) : segment_entry_(segment_entry) {}

void NewTxnSegmentStore::AddDeltaOp(CatalogDeltaEntry *local_delta_ops, AppendState *append_state, NewTxn *txn, bool set_sealed) const {
    TxnTimeStamp commit_ts = txn->CommitTS();

    if (set_sealed) {
        // build minmax filter
        BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(segment_entry_, txn->buffer_mgr(), commit_ts);
        // now have minmax filter and optional bloom filter
        // serialize filter
        local_delta_ops->AddOperation(
            MakeUnique<AddSegmentEntryOp>(segment_entry_, commit_ts, segment_entry_->GetFastRoughFilter()->SerializeToString()));
    } else {
        local_delta_ops->AddOperation(MakeUnique<AddSegmentEntryOp>(segment_entry_, commit_ts));
    }
    for (auto [block_id, block_entry] : block_entries_) {
        if (set_sealed) {
            local_delta_ops->AddOperation(
                MakeUnique<AddBlockEntryOp>(block_entry, commit_ts, block_entry->GetFastRoughFilter()->SerializeToString()));
        } else {
            local_delta_ops->AddOperation(MakeUnique<AddBlockEntryOp>(block_entry, commit_ts));
        }
        for (const auto &column_entry : block_entry->columns()) {
            local_delta_ops->AddOperation(MakeUnique<AddColumnEntryOp>(column_entry.get(), commit_ts));
        }
    }

    for (auto *block_column_entry : block_column_entries_) {
        local_delta_ops->AddOperation(MakeUnique<AddColumnEntryOp>(block_column_entry, commit_ts));
    }
}

///-----------------------------------------------------------------------------

NewTxnIndexStore::NewTxnIndexStore(TableIndexEntry *table_index_entry) : table_index_entry_(table_index_entry) {}

void NewTxnIndexStore::AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnTimeStamp commit_ts) const {
    for (auto [segment_id, segment_index_entry] : index_entry_map_) {
        local_delta_ops->AddOperation(MakeUnique<AddSegmentIndexEntryOp>(segment_index_entry, commit_ts));
    }
    for (auto &[chunk_encode, chunk_index_entry] : chunk_index_entries_) {
        local_delta_ops->AddOperation(MakeUnique<AddChunkIndexEntryOp>(chunk_index_entry, commit_ts));
    }
}

void NewTxnIndexStore::Commit(TransactionID txn_id, TxnTimeStamp commit_ts) {
    for (auto [segment_index_entry, new_chunk, old_chunks] : optimize_data_) {
        segment_index_entry->CommitOptimize(new_chunk, old_chunks, commit_ts);
        chunk_index_entries_.emplace(new_chunk->encode(), new_chunk);
        for (auto *old_chunk : old_chunks) {
            chunk_index_entries_.emplace(old_chunk->encode(), old_chunk);
        }
        index_entry_map_.emplace(segment_index_entry->segment_id(), segment_index_entry);
    }
    for (const auto &[segment_id, segment_index_entry] : index_entry_map_) {
        segment_index_entry->CommitSegmentIndex(txn_id, commit_ts);
    }
    for (auto &[chunk_encode, chunk_index_entry] : chunk_index_entries_) {
        // uncommitted: insert or import, create index
        // committed: create index, insert or import
        if (!chunk_index_entry->Committed()) {
            chunk_index_entry->Commit(commit_ts);
        }
    }
}

void NewTxnIndexStore::Rollback(TxnTimeStamp abort_ts) {
    for (auto [segment_index_entry, new_chunk, old_chunks] : optimize_data_) {
        for (ChunkIndexEntry *old_chunk : old_chunks) {
            old_chunk->DeprecateChunk(abort_ts);
        }
    }
}

void NewTxnIndexStore::AddSegmentOptimizing(SegmentIndexEntry *segment_index_entry) {
    status_ = NewTxnStoreStatus::kOptimizing;
    index_entry_map_.emplace(segment_index_entry->segment_id(), segment_index_entry);
}

bool NewTxnIndexStore::TryRevert() {
    if (status_ == NewTxnStoreStatus::kNone) {
        return false;
    }
    for (auto &[segment_id, segment_index_entry] : index_entry_map_) {
        segment_index_entry->ResetOptimizing();
    }
    status_ = NewTxnStoreStatus::kNone;
    return true;
}

///-----------------------------------------------------------------------------

NewTxnCompactStore::NewTxnCompactStore() : type_(CompactStatementType::kInvalid) {}

NewTxnCompactStore::NewTxnCompactStore(CompactStatementType type) : type_(type) {}

///-----------------------------------------------------------------------------

Pair<std::shared_lock<std::shared_mutex>, const HashMap<String, UniquePtr<NewTxnIndexStore>> &> NewTxnTableStore::txn_indexes_store() const {
    std::shared_lock lock{txn_table_store_mtx_};
    return std::make_pair(std::move(lock), std::cref(txn_indexes_store_));
}

NewTxnTableStore::NewTxnTableStore(NewTxn *txn, const String &table_name) : txn_(txn), table_name_(table_name) {}

NewTxnTableStore::~NewTxnTableStore() = default;

Tuple<UniquePtr<String>, Status> NewTxnTableStore::Import(SharedPtr<SegmentEntry> segment_entry, NewTxn *txn) {
    // this->AddSegmentStore(segment_entry.get());
    // this->AddSealedSegment(segment_entry.get());
    // this->flushed_segments_.emplace_back(segment_entry.get());
    // table_entry_->Import(std::move(segment_entry), txn);

    // has_update_ = true;
    return {nullptr, Status::OK()};
}

Tuple<UniquePtr<String>, Status> NewTxnTableStore::Append(const SharedPtr<DataBlock> &input_block) { return {nullptr, Status::OK()}; }

void NewTxnTableStore::AddIndexStore(TableIndexEntry *table_index_entry) {
    std::lock_guard lock(txn_table_store_mtx_);

    txn_indexes_.emplace(table_index_entry, ptr_seq_n_++);
    has_update_ = true;
}

void NewTxnTableStore::AddSegmentIndexesStore(TableIndexEntry *table_index_entry, const Vector<SegmentIndexEntry *> &segment_index_entries) {
    std::lock_guard lock(txn_table_store_mtx_);

    auto *txn_index_store = this->GetIndexStore(table_index_entry, false);
    for (auto *segment_index_entry : segment_index_entries) {
        auto [iter, insert_ok] = txn_index_store->index_entry_map_.emplace(segment_index_entry->segment_id(), segment_index_entry);
        if (!insert_ok) {
            String error_message = fmt::format("Attempt to add segment index of segment {} store twice", segment_index_entry->segment_id());
            UnrecoverableError(error_message);
        }
    }

    has_update_ = true;
}

void NewTxnTableStore::AddChunkIndexStore(TableIndexEntry *table_index_entry, ChunkIndexEntry *chunk_index_entry) {
    std::lock_guard lock(txn_table_store_mtx_);

    auto *txn_index_store = this->GetIndexStore(table_index_entry, false);
    SegmentIndexEntry *segment_index_entry = chunk_index_entry->segment_index_entry_;
    txn_index_store->index_entry_map_.emplace(segment_index_entry->segment_id(), segment_index_entry);
    txn_index_store->chunk_index_entries_.emplace(chunk_index_entry->encode(), chunk_index_entry);

    has_update_ = true;
}

void NewTxnTableStore::DropIndexStore(TableIndexEntry *table_index_entry) {
    std::lock_guard lock(txn_table_store_mtx_);

    if (txn_indexes_.contains(table_index_entry)) {
        table_index_entry->Cleanup();
        txn_indexes_.erase(table_index_entry);
        txn_indexes_store_.erase(*table_index_entry->GetIndexName());
    } else {
        txn_indexes_.emplace(table_index_entry, ptr_seq_n_++);
    }

    has_update_ = true;
}

NewTxnIndexStore *NewTxnTableStore::GetIndexStore(TableIndexEntry *table_index_entry, bool need_lock) {
    std::unique_lock lock(txn_table_store_mtx_, std::defer_lock);
    if (need_lock) {
        lock.lock();
    }
    const String &index_name = *table_index_entry->GetIndexName();
    if (auto iter = txn_indexes_store_.find(index_name); iter != txn_indexes_store_.end()) {
        return iter->second.get();
    }
    auto txn_index_store = MakeUnique<NewTxnIndexStore>(table_index_entry);
    auto *ptr = txn_index_store.get();
    txn_indexes_store_.emplace(index_name, std::move(txn_index_store));

    has_update_ = true;
    return ptr;
}

Tuple<UniquePtr<String>, Status> NewTxnTableStore::Delete(const Vector<RowID> &row_ids) { return {nullptr, Status::OK()}; }

void NewTxnTableStore::SetCompactType(CompactStatementType type) { compact_state_.type_ = type; }

Tuple<UniquePtr<String>, Status> NewTxnTableStore::Compact(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data,
                                                           CompactStatementType type) {
    if (compact_state_.type_ != type) {
        String error_message = fmt::format("Compact type mismatch: {} vs {}", static_cast<int>(compact_state_.type_), static_cast<int>(type));
        UnrecoverableError(error_message);
    }
    for (auto &[new_segment, old_segments] : segment_data) {
        auto txn_segment_store = NewTxnSegmentStore::AddSegmentStore(new_segment.get());
        compact_state_.compact_data_.emplace_back(std::move(txn_segment_store), old_segments);
        for (auto *old_segment : old_segments) {
            txn_segments_store_.emplace(old_segment->segment_id(), NewTxnSegmentStore(old_segment));
        }
        this->AddSegmentStore(new_segment.get());
        this->AddSealedSegment(new_segment.get());
        this->flushed_segments_.emplace_back(new_segment.get());
        table_entry_->AddCompactNew(std::move(new_segment));
    }

    has_update_ = true;
    return {nullptr, Status::OK()};
}

void NewTxnTableStore::Rollback(TransactionID txn_id, TxnTimeStamp abort_ts) {
    std::shared_lock lock(txn_table_store_mtx_);
    if (append_state_.get() != nullptr) {
        // Rollback the data already been appended.
        Catalog::RollbackAppend(table_entry_, txn_id, abort_ts, this);
        LOG_TRACE(fmt::format("Rollback prepare appended data in table: {}", *table_entry_->GetTableName()));
    }
    // for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
    //     Catalog::RollbackPopulateIndex(txn_index_store.get(), txn_);
    // }
    // TODO: adapt
    //    Catalog::RollbackCompact(table_entry_, txn_id, abort_ts, compact_state_);
    for (const auto &[new_segment_store, old_segments] : compact_state_.compact_data_) {
        std::move(*new_segment_store.segment_entry_).Cleanup();
    }
    // blocks_.clear();

    for (auto &[table_index_entry, ptr_seq_n] : txn_indexes_) {
        table_index_entry->Cleanup();
        Catalog::RemoveIndexEntry(table_index_entry, txn_id); // fix me
    }
    for (const auto &[index_name, txn_index_store] : txn_indexes_store_) {
        txn_index_store->Rollback(abort_ts);
    }
}

bool NewTxnTableStore::CheckConflict(Catalog *catalog, NewTxn *txn) const {
    std::shared_lock lock(txn_table_store_mtx_);
    const String &db_name = txn->db_name();
    const String &table_name = *table_entry_->GetTableName();
    for (const auto &[index_name, index_store] : txn_indexes_store_) {
        auto [table_index_entry1, status] = catalog->GetIndexByName(db_name, table_name, index_name, txn->TxnID(), txn->CommitTS());
        if (!status.ok() || table_index_entry1 != index_store->table_index_entry_) {
            return true;
        }
    }
    for (const auto &[segment_id, block_map] : delete_state_->rows_) {
        if (auto *segment_entry = table_entry_->GetSegmentEntry(segment_id); segment_entry != nullptr) {
            for (const auto &[block_id, block_offsets] : block_map) {
                if (auto block_entry = segment_entry->GetBlockEntryByID(block_id); block_entry.get() != nullptr) {
                    if (block_entry->CheckDeleteConflict(block_offsets, txn->CommitTS())) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

Optional<String> NewTxnTableStore::CheckConflict(const NewTxnTableStore *other_table_store) const {
    std::shared_lock lock_this(txn_table_store_mtx_, std::defer_lock);
    std::shared_lock lock_other(other_table_store->txn_table_store_mtx_, std::defer_lock);
    std::lock(lock_this, lock_other);
    {
        Set<std::string_view> other_txn_indexes_set;
        Map<std::string_view, Set<SegmentID>> other_txn_indexes_store_map;
        for (const auto &index_entry : std::views::keys(other_table_store->txn_indexes_)) {
            other_txn_indexes_set.insert(*index_entry->GetIndexName());
        }
        for (const auto &[index_name, index_store] : other_table_store->txn_indexes_store_) {
            auto &segment_set = other_txn_indexes_store_map[index_name];
            for (const auto segment_id : std::views::keys(index_store->index_entry_map_)) {
                segment_set.insert(segment_id);
            }
        }
        for (const auto &index_entry : std::views::keys(txn_indexes_)) {
            if (const auto &index_name = *index_entry->GetIndexName(); other_txn_indexes_set.contains(index_name)) {
                return fmt::format("{}: txn_indexes_ containing Index {} conflict with other_table_store->txn_indexes_", __func__, index_name);
            }
        }
        for (const auto &[index_name, index_store] : txn_indexes_store_) {
            if (other_txn_indexes_set.contains(index_name)) {
                return fmt::format("{}: txn_indexes_store_ containing Index {} conflict with other_table_store->txn_indexes_", __func__, index_name);
            }
            if (const auto iter = other_txn_indexes_store_map.find(index_name); iter != other_txn_indexes_store_map.end()) {
                for (const auto &other_segment_set = iter->second; const auto segment_id : std::views::keys(index_store->index_entry_map_)) {
                    if (other_segment_set.contains(segment_id)) {
                        return fmt::format(
                            "{}: txn_indexes_store_ containing Index {} Segment {} conflict with other_table_store->txn_indexes_store_",
                            __func__,
                            index_name,
                            segment_id);
                    }
                }
            }
        }
    }

    if (!delete_state_ || !other_table_store->delete_state_) {
        return None;
    }
    const auto &delete_state = *delete_state_;
    const auto &other_delete_state = *other_table_store->delete_state_;
    for (const auto &[segment_id, block_map] : delete_state.rows_) {
        auto other_iter = other_delete_state.rows_.find(segment_id);
        if (other_iter == other_delete_state.rows_.end()) {
            continue;
        }
        for (const auto &[block_id, block_offsets] : block_map) {
            auto other_block_iter = other_iter->second.find(block_id);
            if (other_block_iter == other_iter->second.end()) {
                continue;
            }
            const auto &other_block_offsets = other_block_iter->second;
            SizeT j = 0;
            for (const auto &block_offset : block_offsets) {
                while (j < other_block_offsets.size() && other_block_offsets[j] < block_offset) {
                    ++j;
                }
                if (j == other_block_offsets.size()) {
                    break;
                }
                if (other_block_offsets[j] == block_offset) {
                    return fmt::format("Delete conflict: segment_id: {}, block_id: {}, block_offset: {}", segment_id, block_id, block_offset);
                }
            }
        }
    }
    return None;
}

void NewTxnTableStore::PrepareCommit1(const Vector<WalSegmentInfo *> &segment_infos) const {
    TxnTimeStamp commit_ts = txn_->CommitTS();
    for (auto *segment_entry : flushed_segments_) {
        WalSegmentInfo *segment_info_ptr = nullptr;
        for (auto *segment_info : segment_infos) {
            if (segment_info->segment_id_ == segment_entry->segment_id()) {
                segment_info_ptr = segment_info;
                break;
            }
        }
        if (segment_info_ptr == nullptr) {
            String error_message = fmt::format("Segment info not found, segment id: {}", segment_entry->segment_id());
            UnrecoverableError(error_message);
        }
        segment_entry->CommitFlushed(commit_ts, segment_info_ptr);
    }
}

// TODO: remove commit_ts
void NewTxnTableStore::PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {
    // Init append state
    // append_state_ = MakeUnique<AppendState>(this->blocks_);

    // Start to append
    LOG_TRACE(fmt::format("Transaction local storage table: {}, Start to prepare commit", *table_entry_->GetTableName()));
    Catalog::Append(table_entry_, txn_id, this, commit_ts, buffer_mgr);

    // Attention: "compact" needs to be ahead of "delete"
    // TODO: Adapt
    //    if (compact_state_.type_ != CompactStatementType::kInvalid) {
    //        LOG_DEBUG(fmt::format("Commit compact, table dir: {}, commit ts: {}", *table_entry_->TableEntryDir(), commit_ts));
    //        Catalog::CommitCompact(table_entry_, txn_id, commit_ts, compact_state_);
    //    }

    Catalog::Delete(table_entry_, txn_id, this, commit_ts, *delete_state_);

    for (auto *sealed_segment : set_sealed_segments_) {
        if (!sealed_segment->SetSealed()) {
            String error_message = fmt::format("Set sealed segment failed, segment id: {}", sealed_segment->segment_id());
            UnrecoverableError(error_message);
        }
    }

    LOG_TRACE(fmt::format("Transaction local storage table: {}, Complete commit preparing", *table_entry_->GetTableName()));
}

/**
 * @brief Call for really commit the data to disk.
 */
void NewTxnTableStore::Commit(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::shared_lock lock(txn_table_store_mtx_);
    // TODO: adapt
    //    Catalog::CommitWrite(table_entry_, txn_id, commit_ts, txn_segments_store_, &delete_state_);
    for (const auto &[index_name, txn_index_store] : txn_indexes_store_) {
        //        Catalog::CommitCreateIndex(txn_index_store.get(), commit_ts);
        txn_index_store->Commit(txn_id, commit_ts);
    }
    for (auto [table_index_entry, ptr_seq_n] : txn_indexes_) {
        table_index_entry->Commit(commit_ts);
        switch (table_index_entry->index_base()->index_type_) {
            case IndexType::kFullText: {
                table_index_entry->UpdateFulltextSegmentTs(commit_ts);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void NewTxnTableStore::MaintainCompactionAlg() {
    for (auto *sealed_segment : set_sealed_segments_) {
        table_entry_->AddSegmentToCompactionAlg(sealed_segment);
    }
    for (const auto &[segment_id, delete_map] : delete_state_->rows_) {
        table_entry_->AddDeleteToCompactionAlg(segment_id);
    }

    has_update_ = true;
}

void NewTxnTableStore::AddSegmentStore(SegmentEntry *segment_entry) {
    auto [iter, insert_ok] = txn_segments_store_.emplace(segment_entry->segment_id(), NewTxnSegmentStore::AddSegmentStore(segment_entry));
    if (!insert_ok) {
        String error_message = fmt::format("Attempt to add segment store twice");
        UnrecoverableError(error_message);
    }
    has_update_ = true;
}

void NewTxnTableStore::AddBlockStore(SegmentEntry *segment_entry, BlockEntry *block_entry) {
    auto iter = txn_segments_store_.find(segment_entry->segment_id());
    if (iter == txn_segments_store_.end()) {
        iter = txn_segments_store_.emplace(segment_entry->segment_id(), NewTxnSegmentStore(segment_entry)).first;
    }
    iter->second.block_entries_.emplace(block_entry->block_id(), block_entry);
    has_update_ = true;
}

void NewTxnTableStore::AddBlockColumnStore(SegmentEntry *segment_entry, BlockEntry *block_entry, BlockColumnEntry *block_column_entry) {
    auto iter = txn_segments_store_.find(segment_entry->segment_id());
    if (iter == txn_segments_store_.end()) {
        iter = txn_segments_store_.emplace(segment_entry->segment_id(), NewTxnSegmentStore(segment_entry)).first;
    }
    iter->second.block_column_entries_.emplace_back(block_column_entry);
    has_update_ = true;
}

void NewTxnTableStore::AddSealedSegment(SegmentEntry *segment_entry) {
    set_sealed_segments_.emplace(segment_entry);
    has_update_ = true;
}

void NewTxnTableStore::AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnManager *txn_mgr, TxnTimeStamp commit_ts, bool added) const {
    std::shared_lock lock(txn_table_store_mtx_);
    if (!has_update_) {
        // No any option
        LOG_TRACE("Not update on txn table store, no need to add delta op");
        return;
    }

    if (!added) {
        local_delta_ops->AddOperation(MakeUnique<AddTableEntryOp>(table_entry_, commit_ts));
    }

    Vector<Pair<TableIndexEntry *, int>> txn_indexes_vec(txn_indexes_.begin(), txn_indexes_.end());
    std::sort(txn_indexes_vec.begin(), txn_indexes_vec.end(), [](const auto &lhs, const auto &rhs) { return lhs.second < rhs.second; });
    for (auto [table_index_entry, ptr_seq_n] : txn_indexes_vec) {
        local_delta_ops->AddOperation(MakeUnique<AddTableIndexEntryOp>(table_index_entry, commit_ts));
    }
    for (const auto &[index_name, index_store] : txn_indexes_store_) {
        index_store->AddDeltaOp(local_delta_ops, commit_ts);
    }
    for (const auto &[segment_id, segment_store] : txn_segments_store_) {
        bool set_sealed = set_sealed_segments_.contains(segment_store.segment_entry_);
        segment_store.AddDeltaOp(local_delta_ops, append_state_.get(), txn_, set_sealed);
    }
}

void NewTxnTableStore::TryRevert() {
    std::shared_lock lock(txn_table_store_mtx_);
    if (table_status_ == NewTxnStoreStatus::kCompacting) {
        table_status_ = NewTxnStoreStatus::kNone;
        table_entry_->SetCompactDone();
    } else if (table_status_ == NewTxnStoreStatus::kCreatingIndex) {
        table_status_ = NewTxnStoreStatus::kNone;
        table_entry_->SetCreateIndexDone();
    }
    if (added_txn_num_) {
        added_txn_num_ = false;
        table_entry_->DecWriteTxnNum();
    }
    for (const auto &[index_name, index_store] : txn_indexes_store_) {
        index_store->TryRevert();
    }
}

NewTxnTableStore1::NewTxnTableStore1() = default;

NewTxnTableStore1::~NewTxnTableStore1() = default;

Status NewTxnTableStore1::Append(const SharedPtr<DataBlock> &input_block) {
    if (input_block->row_count() == 0) {
        UnrecoverableError("Attempt to append empty data block into transaction table store");
    }

    // const Vector<SharedPtr<DataType>> &column_types = input_block->types();

    // DataBlock *current_block{nullptr};
    if (append_state_ == nullptr) {
        append_state_ = MakeUnique<AppendState>();
    }

    // if (append_state_->blocks_.empty()) {
    //     append_state_->blocks_.emplace_back(DataBlock::Make());
    //     append_state_->blocks_.back()->Init(column_types);
    // }
    // current_block = append_state_->blocks_.back().get();

    // if (current_block->row_count() + input_block->row_count() > current_block->capacity()) {
    //     SizeT to_append = current_block->capacity() - current_block->row_count();
    //     current_block->AppendWith(input_block.get(), 0, to_append);
    //     current_block->Finalize();

    //     append_state_->blocks_.emplace_back(DataBlock::Make());
    //     append_state_->blocks_.back()->Init(column_types);
    //     current_block = append_state_->blocks_.back().get();
    //     current_block->AppendWith(input_block.get(), to_append, input_block->row_count() - to_append);
    // } else {
    //     SizeT to_append = input_block->row_count();
    //     current_block->AppendWith(input_block.get(), 0, to_append);
    // }
    // current_block->Finalize();

    append_state_->blocks_.emplace_back(input_block);

    return Status::OK();
}

Status NewTxnTableStore1::Delete(const Vector<RowID> &row_ids) {
    if (!delete_state_) {
        delete_state_ = MakeUnique<DeleteState>();
    }
    HashMap<SegmentID, HashMap<BlockID, Vector<BlockOffset>>> &row_hash_table = delete_state_->rows_;
    for (auto row_id : row_ids) {
        BlockID block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        auto &seg_map = row_hash_table[row_id.segment_id_];
        auto &block_vec = seg_map[block_id];
        block_vec.emplace_back(block_offset);
        if (block_vec.size() > DEFAULT_BLOCK_CAPACITY) {
            String error_message = "Delete row exceed block capacity";
            UnrecoverableError(error_message);
        }
    }

    return Status::OK();
}

void NewTxnTableStore1::GetAccessState(const Vector<RowID> &row_ids, AccessState &access_state) {
    HashMap<SegmentID, HashMap<BlockID, Vector<BlockOffset>>> &row_hash_table = access_state.rows_;
    for (auto row_id : row_ids) {
        BlockID block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        auto &seg_map = row_hash_table[row_id.segment_id_];
        auto &block_vec = seg_map[block_id];
        block_vec.emplace_back(block_offset);
        if (block_vec.size() > DEFAULT_BLOCK_CAPACITY) {
            String error_message = "Delete row exceed block capacity";
            UnrecoverableError(error_message);
        }
    }
}

DeleteState &NewTxnTableStore1::undo_delete_state() { 
    if (!undo_delete_state_) {
        undo_delete_state_ = MakeUnique<DeleteState>();
    }
    return *undo_delete_state_;
}

NewTxnStore::NewTxnStore(NewTxn *txn) : txn_(txn) {}

void NewTxnStore::AddDBStore(DBEntry *db_entry) { txn_dbs_.emplace(db_entry, ptr_seq_n_++); }

void NewTxnStore::DropDBStore(DBEntry *dropped_db_entry) {
    if (txn_dbs_.contains(dropped_db_entry)) {
        dropped_db_entry->Cleanup();
        txn_dbs_.erase(dropped_db_entry);
    } else {
        txn_dbs_.emplace(dropped_db_entry, ptr_seq_n_++);
    }
}

void NewTxnStore::AddTableStore(TableEntry *table_entry) { txn_tables_.emplace(table_entry, ptr_seq_n_++); }

void NewTxnStore::DropTableStore(TableEntry *dropped_table_entry) {
    if (txn_tables_.contains(dropped_table_entry)) {
        txn_tables_.erase(dropped_table_entry);
        dropped_table_entry->Cleanup();
        txn_tables_store_.erase(*dropped_table_entry->GetTableName());
    } else {
        txn_tables_.emplace(dropped_table_entry, ptr_seq_n_++);
    }
}

NewTxnTableStore *NewTxnStore::GetNewTxnTableStore(const String &table_name) {
    if (auto iter = txn_tables_store_.find(table_name); iter != txn_tables_store_.end()) {
        return iter->second.get();
    }
    auto [iter, insert_ok] = txn_tables_store_.emplace(table_name, MakeUnique<NewTxnTableStore>(txn_, table_name));
    return iter->second.get();
}

NewTxnTableStore *NewTxnStore::GetExistNewTxnTableStore(TableEntry *table_entry) const {
    const String &table_name = *table_entry->GetTableName();
    auto iter = txn_tables_store_.find(table_name);
    if (iter == txn_tables_store_.end()) {
        UnrecoverableError("txn table store not exist");
    }
    return iter->second.get();
}

void NewTxnStore::AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnManager *txn_mgr) const {
    TxnTimeStamp commit_ts = txn_->CommitTS();
    Vector<Pair<DBEntry *, int>> txn_dbs_vec(txn_dbs_.begin(), txn_dbs_.end());
    std::sort(txn_dbs_vec.begin(), txn_dbs_vec.end(), [](const auto &lhs, const auto &rhs) { return lhs.second < rhs.second; });
    for (auto [db_entry, ptr_seq_n] : txn_dbs_vec) {
        local_delta_ops->AddOperation(MakeUnique<AddDBEntryOp>(db_entry, commit_ts));
    }

    Vector<Pair<TableEntry *, int>> txn_tables_vec(txn_tables_.begin(), txn_tables_.end());
    std::sort(txn_tables_vec.begin(), txn_tables_vec.end(), [](const auto &lhs, const auto &rhs) { return lhs.second < rhs.second; });
    for (auto [table_entry, ptr_seq_n] : txn_tables_vec) {
        local_delta_ops->AddOperation(MakeUnique<AddTableEntryOp>(table_entry, commit_ts));
    }
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        bool added = txn_tables_.contains(table_store->GetTableEntry());
        table_store->AddDeltaOp(local_delta_ops, txn_mgr, commit_ts, added);
    }
}

void NewTxnStore::MaintainCompactionAlg() const {
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->MaintainCompactionAlg();
    }
}

bool NewTxnStore::CheckConflict(Catalog *catalog) {
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        const String &db_name = *table_store->GetTableEntry()->GetDBName();
        txn_->SetDBName(db_name);
        auto [table_entry1, status] = catalog->GetTableByName(db_name, table_name, txn_->TxnID(), txn_->CommitTS());
        if (!status.ok() || table_entry1 != table_store->GetTableEntry()) {
            return true;
        }
        if (table_store->CheckConflict(catalog, txn_)) {
            return true;
        }
    }
    return false;
}

Optional<String> NewTxnStore::CheckConflict(const NewTxnStore &other_txn_store) {
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        for (const auto [table_entry, _] : other_txn_store.txn_tables_) {
            if (table_name == *table_entry->GetTableName()) {
                return fmt::format("txn_tables_store_ containing table_name {} conflict with other_txn_store.txn_tables_", table_name);
            }
        }

        auto other_iter = other_txn_store.txn_tables_store_.find(table_name);
        if (other_iter == other_txn_store.txn_tables_store_.end()) {
            continue;
        }

        const NewTxnTableStore *other_table_store = other_iter->second.get();
        if (const auto conflict_reason = table_store->CheckConflict(other_table_store); conflict_reason) {
            return fmt::format("txn_tables_store_ containing table_name {} conflict with other_txn_store.txn_tables_store_: {}",
                               table_name,
                               *conflict_reason);
        }
    }
    return None;
}

void NewTxnStore::PrepareCommit1() {
    if (txn_ == nullptr) {
        // New txn branch
        return;
    }
    WalEntry *wal_entry = txn_->GetWALEntry();
    Vector<WalSegmentInfo *> segment_infos;
    for (auto &cmd : wal_entry->cmds_) {
        if (cmd->GetType() == WalCommandType::IMPORT) {
            auto *import_cmd = static_cast<WalCmdImportV2 *>(cmd.get());
            segment_infos.emplace_back(&import_cmd->segment_info_);
        } else if (cmd->GetType() == WalCommandType::COMPACT) {
            auto *compact_cmd = static_cast<WalCmdCompactV2 *>(cmd.get());
            for (auto &segment_info : compact_cmd->new_segment_infos_) {
                segment_infos.emplace_back(&segment_info);
            }
        }
    }
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->PrepareCommit1(segment_infos);
    }
    if (!segment_infos.empty()) {
        PersistenceManager *pm = InfinityContext::instance().persistence_manager();
        if (pm != nullptr) {
            PersistResultHandler handler(pm);
            PersistWriteResult result = pm->CurrentObjFinalize(true);
            handler.HandleWriteResult(result);
        }
        LOG_TRACE("Finalize current object to ensure PersistenceManager be in a consistent state");
    }
}

void NewTxnStore::PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->PrepareCommit(txn_id, commit_ts, buffer_mgr);
    }
}

void NewTxnStore::CommitBottom(TransactionID txn_id, TxnTimeStamp commit_ts) {
    // Commit the prepared data
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->Commit(txn_id, commit_ts);
    }

    // Commit databases to memory catalog
    for (auto [db_entry, ptr_seq_n] : txn_dbs_) {
        db_entry->Commit(commit_ts);
    }

    // Commit tables to memory catalog
    for (auto [table_entry, ptr_seq_n] : txn_tables_) {
        table_entry->Commit(commit_ts);
    }
}

void NewTxnStore::Rollback(TransactionID txn_id, TxnTimeStamp abort_ts) {
    // Rollback the prepared data
    for (const auto &name_table_pair : txn_tables_store_) {
        NewTxnTableStore *table_local_store = name_table_pair.second.get();
        table_local_store->Rollback(txn_id, abort_ts);
    }

    for (auto [table_entry, ptr_seq_n] : txn_tables_) {
        table_entry->Cleanup();
        Catalog::RemoveTableEntry(table_entry, txn_id);
    }

    Catalog *catalog_ptr = InfinityContext::instance().storage()->catalog();
    for (auto [db_entry, ptr_seq_n] : txn_dbs_) {
        db_entry->Cleanup();

        catalog_ptr->RemoveDBEntry(db_entry, txn_id);
    }
}

bool NewTxnStore::ReadOnly() const {
    bool read_only = true;
    if (!txn_dbs_.empty()) {
        // CREATE or DROP DB
        read_only = false;
    }
    if (!txn_tables_.empty()) {
        // CREATE or DROP TABLE
        read_only = false;
    }
    if (!txn_tables_store_.empty()) {
        for (const auto &txn_table_store : txn_tables_store_) {
            if (txn_table_store.second->HasUpdate()) {
                read_only = false;
                break;
            }
        }
        //        read_only = false;
    }

    return read_only;
}

void NewTxnStore::RevertTableStatus() {
    for (const auto &[table_name, table_store] : txn_tables_store_) {
        table_store->TryRevert();
    }
}

void NewTxnStore::SetCompacting(TableEntry *table_entry) { GetNewTxnTableStore(*table_entry->GetTableName())->SetCompacting(); }

void NewTxnStore::SetCreatingIndex(TableEntry *table_entry) { GetNewTxnTableStore(*table_entry->GetTableName())->SetCreatingIndex(); }

WalCmdDumpIndexV2 *NewTxnStore::GetDumpIndexCmd(const String &idx_segment_key) {
    std::unique_lock lock(mtx_);
    auto iter = dump_index_cmds_.find(idx_segment_key);
    if (iter != dump_index_cmds_.end()) {
        return iter->second;
    }
    return nullptr;
}

bool NewTxnStore::AddDumpIndexCmd(String idx_segment_key, WalCmdDumpIndexV2 *dump_index_cmd) {
    std::unique_lock lock(mtx_);
    bool add_success = dump_index_cmds_.emplace(std::move(idx_segment_key), dump_index_cmd).second;
    return add_success;
}

NewTxnTableStore1 *NewTxnStore::GetNewTxnTableStore1(const String &db_id_str, const String &table_id_str) {
    auto iter = txn_tables_store1_.find(table_id_str);
    if (iter == txn_tables_store1_.end()) {
        iter = txn_tables_store1_.emplace(table_id_str, MakeUnique<NewTxnTableStore1>()).first;
    }
    return iter->second.get();
}

} // namespace infinity
