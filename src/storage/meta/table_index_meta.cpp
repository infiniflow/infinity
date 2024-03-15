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

module table_index_meta;

import stl;

import index_base;
import txn_manager;
import default_values;
import txn_state;
import logger;
import third_party;

import infinity_exception;
import status;
import iresearch_datastore;
import extra_ddl_info;
import local_file_system;
import txn;

namespace infinity {

struct SegmentEntry;

TableIndexMeta::TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name)
    : index_name_(std::move(index_name)), table_entry_(table_entry) {}

UniquePtr<TableIndexMeta> TableIndexMeta::NewTableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name) {
    auto table_index_meta = MakeUnique<TableIndexMeta>(table_entry, index_name);
    return table_index_meta;
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::CreateTableIndexEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                                       const SharedPtr<IndexBase> &index_base,
                                                                       ConflictType conflict_type,
                                                                       TransactionID txn_id,
                                                                       TxnTimeStamp begin_ts,
                                                                       TxnManager *txn_mgr,
                                                                       bool is_replay,
                                                                       String replay_table_index_dir) {
    auto init_index_entry = [&]() {
        Txn *txn{nullptr};
        if (txn_mgr != nullptr) {
            txn = txn_mgr->GetTxn(txn_id);
        }
        return TableIndexEntry::NewTableIndexEntry(index_base, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
    };
    return index_entry_list_.AddEntry(std::move(r_lock), std::move(init_index_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableIndexEntry>, Status> TableIndexMeta::DropTableIndexEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                                              ConflictType conflict_type,
                                                                              TransactionID txn_id,
                                                                              TxnTimeStamp begin_ts,
                                                                              TxnManager *txn_mgr) {
    auto init_drop_entry = [&]() {
        auto drop_entry = TableIndexEntry::NewDropTableIndexEntry(this, txn_id, begin_ts);
        drop_entry->deleted_ = true;
        return drop_entry;
    };
    return index_entry_list_.DropEntry(std::move(r_lock), std::move(init_drop_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

void TableIndexMeta::DeleteNewEntry(TransactionID txn_id) { auto erase_list = index_entry_list_.DeleteEntry(txn_id); }

Tuple<SharedPtr<TableIndexInfo>, Status>
TableIndexMeta::GetTableIndexInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_index_entry, status] = index_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SharedPtr<TableIndexInfo> table_index_info = MakeShared<TableIndexInfo>();
    table_index_info->index_name_ = index_name_;
    table_index_info->index_entry_dir_ = table_index_entry->index_dir();
    table_index_info->segment_index_count_ = table_index_entry->index_by_segment().size();
    table_index_info->index_info_ = MakeShared<String>(table_index_entry->index_base()->ToString());

    return {table_index_info, status};
}

SharedPtr<String> TableIndexMeta::ToString() {
    UnrecoverableError("Not implemented");
    return nullptr;
}

nlohmann::json TableIndexMeta::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;

    Vector<TableIndexEntry *> table_index_entry_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["index_name"] = *this->index_name_;

        table_index_entry_candidates.reserve(this->index_entry_list().size());
        for (const auto &base_entry : this->index_entry_list()) {
            if (base_entry->entry_type_ == EntryType::kDummy) {
                continue;
            }
            if (base_entry->entry_type_ != EntryType::kTableIndex) {
                UnrecoverableError("Unexpected entry type during serialize table index meta");
            }
            if (base_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_index_entry_candidates.push_back((TableIndexEntry *)base_entry.get());
            }
        }
    }

    for (const auto &table_index_entry : table_index_entry_candidates) {
        json_res["index_entries"].emplace_back(table_index_entry->Serialize(max_commit_ts));
    }
    return json_res;
}

UniquePtr<TableIndexMeta>
TableIndexMeta::Deserialize(const nlohmann::json &table_index_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    LOG_TRACE(fmt::format("load index"));

    SharedPtr<String> index_name = MakeShared<String>(table_index_meta_json["index_name"]);
    auto res = MakeUnique<TableIndexMeta>(table_entry, index_name);
    if (table_index_meta_json.contains("index_entries")) {
        auto &entries = table_index_meta_json["index_entries"];
        // traverse reversely because a dummy head has been inserted
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = TableIndexEntry::Deserialize(*iter, res.get(), buffer_mgr, table_entry);
            res->index_entry_list().emplace_front(std::move(entry));
        }
    }
    return res;
}

void TableIndexMeta::Cleanup() { index_entry_list_.Cleanup(); }

bool TableIndexMeta::PickCleanup(CleanupScanner *scanner) { return index_entry_list_.PickCleanup(scanner); }

void TableIndexMeta::PickCleanupBySegments(const Vector<SegmentID> &sorted_segment_ids, CleanupScanner *scanner) {
    index_entry_list_.Iterate([&](auto *table_index_entry) { table_index_entry->PickCleanupBySegments(sorted_segment_ids, scanner); },
                              scanner->visible_ts());
}

} // namespace infinity
