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

export module infinity_core:new_txn_store;

import :stl;

import :status;
import internal_types;
import :index_base;
import extra_ddl_info;
import third_party;
import :meta_key;
import command_statement;

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

    Status Delete(const Vector<RowID> &row_ids);

    void GetAccessState(const Vector<RowID> &row_ids, AccessState &access_state);

    DeleteState &delete_state() const { return *delete_state_; }
    DeleteState &undo_delete_state();

private:
    UniquePtr<DeleteState> delete_state_{};      // Used for commit delete operation
    UniquePtr<DeleteState> undo_delete_state_{}; // Used for rollback delete operation
};

export class NewTxnStore {
public:
    NewTxnStore();

public:
    NewTxnTableStore1 *GetNewTxnTableStore1(const String &db_id_str, const String &table_id_str);

private:
    HashMap<String, UniquePtr<NewTxnTableStore1>> txn_tables_store1_{};
};

} // namespace infinity
