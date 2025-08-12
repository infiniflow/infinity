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

export module infinity_core:view;

import :stl;

import select_statement;
import create_view_info;
import data_type;

// A view means a logical plan
namespace infinity {

export class View {
public:
    explicit View(SharedPtr<CreateViewInfo> create_view_info,
                  SharedPtr<Vector<String>> column_names,
                  SharedPtr<Vector<SharedPtr<DataType>>> column_types)
        : create_view_info_(std::move(create_view_info)), column_names_(std::move(column_names)), column_types_(std::move(column_types)) {}

    [[nodiscard]] inline const String &schema_name() const { return create_view_info_->schema_name_; }

    [[nodiscard]] inline const String &view_name() const { return create_view_info_->view_name_; }

    const SelectStatement *GetSQLStatement() { return create_view_info_->select_; }

    [[nodiscard]] inline const SharedPtr<Vector<String>> &column_names() const { return column_names_; };

    [[nodiscard]] inline const SharedPtr<Vector<SharedPtr<DataType>>> &column_types() const { return column_types_; };

private:
    SharedPtr<CreateViewInfo> create_view_info_{};
    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
};
} // namespace infinity
