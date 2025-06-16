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

void NewTxnStore::AddDBStore(DBEntry *db_entry) {}

void NewTxnStore::DropDBStore(DBEntry *dropped_db_entry) {}

void NewTxnStore::AddTableStore(TableEntry *table_entry) {}

void NewTxnStore::DropTableStore(TableEntry *dropped_table_entry) {}

void NewTxnStore::AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnManager *txn_mgr) const {}

void NewTxnStore::MaintainCompactionAlg() const {}

bool NewTxnStore::CheckConflict(Catalog *catalog) { return false; }

Optional<String> NewTxnStore::CheckConflict(const NewTxnStore &other_txn_store) { return None; }

void NewTxnStore::PrepareCommit1() {}

void NewTxnStore::PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {}

void NewTxnStore::CommitBottom(TransactionID txn_id, TxnTimeStamp commit_ts) {}

void NewTxnStore::Rollback(TransactionID txn_id, TxnTimeStamp abort_ts) {}

bool NewTxnStore::ReadOnly() const { return false; }

void NewTxnStore::RevertTableStatus() {}

void NewTxnStore::SetCompacting(TableEntry *table_entry) {}

void NewTxnStore::SetCreatingIndex(TableEntry *table_entry) {}

NewTxnTableStore1 *NewTxnStore::GetNewTxnTableStore1(const String &db_id_str, const String &table_id_str) {
    auto iter = txn_tables_store1_.find(table_id_str);
    if (iter == txn_tables_store1_.end()) {
        iter = txn_tables_store1_.emplace(table_id_str, MakeUnique<NewTxnTableStore1>()).first;
    }
    return iter->second.get();
}

} // namespace infinity
