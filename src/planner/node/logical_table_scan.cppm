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

export module infinity_core:logical_table_scan;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :base_table_ref;
import :meta_info;
import :fast_rough_filter;

import internal_types;
import data_type;

namespace infinity {

export class LogicalTableScan : public LogicalNode {
public:
    explicit LogicalTableScan(u64 node_id, std::shared_ptr<BaseTableRef> base_table_ref, bool add_row_id = true);

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableInfo *table_info() const;

    [[nodiscard]] std::string TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalTableScan"; }

    std::shared_ptr<BaseTableRef> base_table_ref_{};

    std::unique_ptr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_;

    bool add_row_id_;
};

} // namespace infinity
