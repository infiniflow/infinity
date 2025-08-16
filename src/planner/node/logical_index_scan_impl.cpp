//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module infinity_core:logical_index_scan.impl;

import :logical_index_scan;
import :logical_node_type;
import :logical_node;
import :column_binding;
import :base_table_ref;
import :default_values;
import :base_expression;
import :function_expression;
import :cast_expression;
import :column_expression;

import std;

import logical_type;
import data_type;

namespace infinity {

LogicalIndexScan::LogicalIndexScan(u64 node_id,
                                   std::shared_ptr<BaseTableRef> &&base_table_ref,
                                   std::shared_ptr<BaseExpression> &&index_filter,
                                   std::unique_ptr<IndexFilterEvaluator> &&index_filter_evaluator,
                                   std::unique_ptr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                                   bool add_row_id)
    : LogicalNode(node_id, LogicalNodeType::kIndexScan), base_table_ref_(std::move(base_table_ref)), index_filter_(std::move(index_filter)),
      index_filter_evaluator_(std::move(index_filter_evaluator)), fast_rough_filter_evaluator_(std::move(fast_rough_filter_evaluator)),
      add_row_id_(add_row_id) {}

std::vector<ColumnBinding> LogicalIndexScan::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (size_t col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalIndexScan::GetOutputNames() const {
    auto result_names = std::make_shared<std::vector<std::string>>();
    if (add_row_id_) {
        result_names->emplace_back(COLUMN_NAME_ROW_ID);
    }
    return result_names;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalIndexScan::GetOutputTypes() const {
    auto result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    if (add_row_id_) {
        result_types->emplace_back(std::make_shared<DataType>(LogicalType::kRowID));
    }
    return result_types;
}

TableInfo *LogicalIndexScan::table_info() const { return base_table_ref_->table_info_.get(); }

std::string LogicalIndexScan::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalIndexScan::TableIndex() const { return base_table_ref_->table_index_; }

std::string LogicalIndexScan::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "IndexScan: " << *base_table_ref_->table_info_->table_name_;
    ss << ", filter expression: " << index_filter_->Name();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
