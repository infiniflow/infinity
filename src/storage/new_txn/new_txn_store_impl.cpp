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

module infinity_core:new_txn_store.impl;

import :new_txn_store;
import :stl;
import :status;
import :infinity_exception;
import :data_block;
import :data_access_state;
import :new_txn;
import :default_values;
import :persistence_manager;
import :infinity_context;
import :kv_store;

import std;
import third_party;

import internal_types;
import data_type;
import compact_statement;

namespace infinity {

NewTxnTableStore1::NewTxnTableStore1() = default;

NewTxnTableStore1::~NewTxnTableStore1() = default;

Status NewTxnTableStore1::Delete(const std::vector<RowID> &row_ids) {
    if (!delete_state_) {
        delete_state_ = std::make_unique<DeleteState>();
    }
    std::unordered_map<SegmentID, std::unordered_map<BlockID, std::vector<BlockOffset>>> &row_hash_table = delete_state_->rows_;
    for (auto row_id : row_ids) {
        BlockID block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        auto &seg_map = row_hash_table[row_id.segment_id_];
        auto &block_vec = seg_map[block_id];
        block_vec.emplace_back(block_offset);
        if (block_vec.size() > DEFAULT_BLOCK_CAPACITY) {
            UnrecoverableError("Delete row exceed block capacity");
        }
    }

    return Status::OK();
}

void NewTxnTableStore1::GetAccessState(const std::vector<RowID> &row_ids, AccessState &access_state) {
    std::unordered_map<SegmentID, std::unordered_map<BlockID, std::vector<BlockOffset>>> &row_hash_table = access_state.rows_;
    for (auto row_id : row_ids) {
        BlockID block_id = row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
        auto &seg_map = row_hash_table[row_id.segment_id_];
        auto &block_vec = seg_map[block_id];
        block_vec.emplace_back(block_offset);
        if (block_vec.size() > DEFAULT_BLOCK_CAPACITY) {
            UnrecoverableError("Delete row exceed block capacity");
        }
    }
}

DeleteState &NewTxnTableStore1::undo_delete_state() {
    if (!undo_delete_state_) {

        undo_delete_state_ = std::make_unique<DeleteState>();
    }
    return *undo_delete_state_;
}

NewTxnStore::NewTxnStore() = default;

NewTxnTableStore1 *NewTxnStore::GetNewTxnTableStore1(const std::string &db_id_str, const std::string &table_id_str) {
    auto iter = txn_tables_store1_.find(table_id_str);
    if (iter == txn_tables_store1_.end()) {
        iter = txn_tables_store1_.emplace(table_id_str, std::make_unique<NewTxnTableStore1>()).first;
    }
    return iter->second.get();
}

} // namespace infinity
