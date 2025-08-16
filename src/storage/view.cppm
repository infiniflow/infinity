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

export module infinity_core:view;

import std;

import select_statement;
import create_view_info;
import data_type;

// A view means a logical plan
namespace infinity {

export class View {
public:
    explicit View(std::shared_ptr<CreateViewInfo> create_view_info,
                  std::shared_ptr<std::vector<std::string>> column_names,
                  std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types)
        : create_view_info_(std::move(create_view_info)), column_names_(std::move(column_names)), column_types_(std::move(column_types)) {}

    [[nodiscard]] inline const std::string &schema_name() const { return create_view_info_->schema_name_; }

    [[nodiscard]] inline const std::string &view_name() const { return create_view_info_->view_name_; }

    const SelectStatement *GetSQLStatement() { return create_view_info_->select_; }

    [[nodiscard]] inline const std::shared_ptr<std::vector<std::string>> &column_names() const { return column_names_; };

    [[nodiscard]] inline const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &column_types() const { return column_types_; };

private:
    std::shared_ptr<CreateViewInfo> create_view_info_{};
    std::shared_ptr<std::vector<std::string>> column_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types_{};
};
} // namespace infinity
