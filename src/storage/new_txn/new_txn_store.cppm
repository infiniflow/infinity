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

export module infinity_core:new_txn_store;

import :status;
import :index_base;
import :meta_key;

import std;
import third_party;

import command_statement;
import extra_ddl_info;
import internal_types;

namespace infinity {

export class NewTxn;
class BGTaskProcessor;
struct DataBlock;
class BufferManager;

class KVInstance;
struct WalSegmentInfo;
struct WalCmdDumpIndexV2;

struct DeleteState;
struct AccessState;

export class NewTxnTableStore1 {
public:
    NewTxnTableStore1();

    ~NewTxnTableStore1();

    Status Delete(const std::vector<RowID> &row_ids);

    DeleteState &delete_state() const { return *delete_state_; }
    DeleteState &undo_delete_state();

private:
    std::unique_ptr<DeleteState> delete_state_{};      // Used for commit delete operation
    std::unique_ptr<DeleteState> undo_delete_state_{}; // Used for rollback delete operation
};

export class NewTxnStore {
public:
    NewTxnStore();

public:
    NewTxnTableStore1 *GetNewTxnTableStore1(const std::string &db_id_str, const std::string &table_id_str);

private:
    std::unordered_map<std::string, std::unique_ptr<NewTxnTableStore1>> txn_tables_store1_{};
};

} // namespace infinity
