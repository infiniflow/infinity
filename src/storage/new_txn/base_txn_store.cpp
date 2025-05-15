// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module base_txn_store;

import third_party;

namespace infinity {

String CreateDBTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id:{}, comment: {}", TransactionType2Str(type_), db_name_, db_id_, *comment_ptr_);
}

String DropDBTxnStore::ToString() const { return fmt::format("{}: database: {}, db_id: {}", TransactionType2Str(type_), db_name_, db_id_); }

String CreateTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table_id: {}", TransactionType2Str(type_), db_name_, db_id_, table_id_);
}

String DropTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, table: {}, table_id: {}", TransactionType2Str(type_), db_name_, table_name_, table_id_str_);
}

String RenameTableTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, old_table: {}, table_id: {}, new_table_name: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       old_table_name_,
                       table_id_str_,
                       new_table_name_);
}

String CreateIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_id_);
}

String DropIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_name_,
                       index_id_str_);
}

String AppendTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size());
}

String ImportTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_);
}

String DumpMemIndexTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       index_name_,
                       index_id_str_);
}

String AddColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_,
                       column_defs_.size());
}

String DropColumnsTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       column_names_.size());
}

String CompactTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_str_,
                       table_name_,
                       table_id_str_);
}

String DeleteTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ids_.size());
}

String UpdateTxnStore::ToString() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, appended: {}, deleted: {}",
                       TransactionType2Str(type_),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ranges_.size(),
                       row_ids_.size());
}

} // namespace infinity
