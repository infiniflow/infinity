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

export module view_entry;

import base_entry;
import internal_types;
import stl;
import data_type;
import select_statement;
import create_view_info;
// FIXME: Not finished

namespace infinity {

class ViewMeta;

class Txn;

export struct ViewEntry : public BaseEntry {
public:
    explicit ViewEntry(bool deleted,
                       SharedPtr<CreateViewInfo> create_view_info,
                       SharedPtr<String> view_name,
                       SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                       SharedPtr<Vector<String>> column_names,
                       ViewMeta *view_meta,
                       TransactionID txn_id,
                       TxnTimeStamp begin_ts)
        : BaseEntry(EntryType::kView, deleted, ""), create_view_info_(create_view_info), view_name_(std::move(view_name)),
          column_types_(std::move(column_types)), column_names_(std::move(column_names)), view_meta_(view_meta) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    [[nodiscard]] inline SelectStatement *GetSQLStatement() const { return create_view_info_->select_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> &column_types() { return column_types_; }

    inline SharedPtr<Vector<String>> &column_names() { return column_names_; }

    inline const SharedPtr<String> &view_name() const { return view_name_; }

    Vector<String> GetFilePath(TransactionID txn_id, TxnTimeStamp begin_ts) const final { return Vector<String>(); }

private:
    std::shared_mutex rw_locker_{};

    SharedPtr<CreateViewInfo> create_view_info_;

    SharedPtr<String> view_name_{};

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};

    ViewMeta *view_meta_{};
};

} // namespace infinity
