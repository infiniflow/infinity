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
import table_entry;
import infinity_exception;
import status;
import extra_ddl_info;
import local_file_system;
import txn;
import create_index_info;

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
                                                                       const SharedPtr<String> &table_entry_dir,
                                                                       ConflictType conflict_type,
                                                                       TransactionID txn_id,
                                                                       TxnTimeStamp begin_ts,
                                                                       TxnManager *txn_mgr) {
    auto init_index_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        return TableIndexEntry::NewTableIndexEntry(index_base, false, table_entry_dir, this, txn_id, begin_ts);
    };
    return index_entry_list_.AddEntry(std::move(r_lock), std::move(init_index_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableIndexEntry>, Status> TableIndexMeta::DropTableIndexEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                                              ConflictType conflict_type,
                                                                              SharedPtr<String> index_name,
                                                                              TransactionID txn_id,
                                                                              TxnTimeStamp begin_ts,
                                                                              TxnManager *txn_mgr) {
    auto index_base = MakeShared<IndexBase>(index_name);
    auto init_drop_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        return TableIndexEntry::NewTableIndexEntry(index_base, true, nullptr, this, txn_id, begin_ts);
    };
    return index_entry_list_.DropEntry(std::move(r_lock), std::move(init_drop_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

void TableIndexMeta::DeleteEntry(TransactionID txn_id) { auto erase_list = index_entry_list_.DeleteEntry(txn_id); }

TableIndexEntry *
TableIndexMeta::CreateEntryReplay(std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                  TransactionID txn_id,
                                  TxnTimeStamp begin_ts) {
    auto [entry, status] =
        index_entry_list_.AddEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(this, txn_id, begin_ts); },
                                         txn_id,
                                         begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return entry;
}

void TableIndexMeta::UpdateEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    auto [entry, status] = index_entry_list_.GetEntryReplay(txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    entry->UpdateEntryReplay(txn_id, begin_ts, commit_ts);
}

void TableIndexMeta::DropEntryReplay(std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                     TransactionID txn_id,
                                     TxnTimeStamp begin_ts) {
    auto [dropped_entry, status] =
        index_entry_list_.DropEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(this, txn_id, begin_ts); },
                                          txn_id,
                                          begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

TableIndexEntry *TableIndexMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [entry, status] = index_entry_list_.GetEntryReplay(txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return entry;
}

Tuple<SharedPtr<TableIndexInfo>, Status>
TableIndexMeta::GetTableIndexInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_index_entry, status] = index_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SharedPtr<String> full_index_dir = MakeShared<String>(fmt::format("{}/{}", *table_index_entry->base_dir_, *table_index_entry->index_dir()));
    SharedPtr<TableIndexInfo> table_index_info = MakeShared<TableIndexInfo>();
    table_index_info->index_name_ = index_name_;
    table_index_info->index_entry_dir_ = full_index_dir;
    table_index_info->segment_index_count_ = table_index_entry->index_by_segment().size();

    auto index_base = table_index_entry->index_base();
    table_index_info->index_type_ = MakeShared<String>(IndexInfo::IndexTypeToString(index_base->index_type_));
    table_index_info->index_other_params_ = MakeShared<String>(index_base->BuildOtherParamsString());

    // Append index column names to the third column
    String column_names;
    String column_ids;
    SizeT idx = 0;
    for (auto &column_name : index_base->column_names_) {
        column_names += column_name;
        auto column_id = table_entry_->GetColumnIdByName(column_name);
        column_ids += std::to_string(column_id);
        if (idx < index_base->column_names_.size() - 1) {
            column_names += ",";
            column_ids += ",";
        }
        idx++;
    }
    table_index_info->index_column_names_ = MakeShared<String>(column_names);
    table_index_info->index_column_ids_ = MakeShared<String>(column_ids);

    return {table_index_info, status};
}

SharedPtr<String> TableIndexMeta::ToString() {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
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
            if (base_entry->entry_type_ != EntryType::kTableIndex) {
                String error_message = "Unexpected entry type during serialize table index meta";
                UnrecoverableError(error_message);
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
