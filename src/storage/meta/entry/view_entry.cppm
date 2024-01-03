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

export module catalog:view_entry;

import :base_entry;

import stl;
import parser;

// FIXME: Not finished

namespace infinity {

class ViewMeta;

class Txn;

export struct ViewEntry : public BaseEntry {
public:
    explicit ViewEntry(const SharedPtr<String> &base_dir,
                       SharedPtr<CreateViewInfo> create_view_info,
                       SharedPtr<String> view_name,
                       SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                       SharedPtr<Vector<String>> column_names,
                       ViewMeta *view_meta,
                       u64 txn_id,
                       TxnTimeStamp begin_ts)
        : BaseEntry(EntryType::kView), create_view_info_(create_view_info), base_dir_(base_dir), view_name_(Move(view_name)),
          column_types_(Move(column_types)), column_names_(Move(column_names)), view_meta_(view_meta) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    [[nodiscard]] inline SelectStatement *GetSQLStatement() const { return create_view_info_->select_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> &column_types() { return column_types_; }

    inline SharedPtr<Vector<String>> &column_names() { return column_names_; }

    inline const SharedPtr<String> &view_name() const { return view_name_; }

private:
    RWMutex rw_locker_{};

    SharedPtr<CreateViewInfo> create_view_info_;

    SharedPtr<String> base_dir_{};

    SharedPtr<String> view_name_{};

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};

    ViewMeta *view_meta_{};
};

} // namespace infinity
