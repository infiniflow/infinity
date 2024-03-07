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

module;

#include <memory>
#include <vector>

module logical_index_scan;

import stl;
import logical_node_type;
import logical_node;
import column_binding;
import base_table_ref;
import catalog;
import default_values;
import base_expression;
import function_expression;
import cast_expression;
import column_expression;
import logical_type;

namespace infinity {

LogicalIndexScan::LogicalIndexScan(u64 node_id,
                                   SharedPtr<BaseTableRef> &&base_table_ref,
                                   SharedPtr<BaseExpression> &&index_filter_qualified,
                                   HashMap<ColumnID, TableIndexEntry *> &&column_index_map,
                                   Vector<FilterExecuteElem> &&filter_execute_command,
                                   UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                                   bool add_row_id)
    : LogicalNode(node_id, LogicalNodeType::kIndexScan), base_table_ref_(std::move(base_table_ref)),
      index_filter_qualified_(std::move(index_filter_qualified)), column_index_map_(std::move(column_index_map)),
      filter_execute_command_(std::move(filter_execute_command)), fast_rough_filter_evaluator_(std::move(fast_rough_filter_evaluator)),
      add_row_id_(add_row_id) {}

Vector<ColumnBinding> LogicalIndexScan::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (SizeT col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalIndexScan::GetOutputNames() const {
    auto result_names = MakeShared<Vector<String>>(*base_table_ref_->column_names_); // copy initialize
    if (add_row_id_) {
        result_names->emplace_back(COLUMN_NAME_ROW_ID);
    }
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalIndexScan::GetOutputTypes() const {
    auto result_types = MakeShared<Vector<SharedPtr<DataType>>>(*base_table_ref_->column_types_); // copy initialize
    if (add_row_id_) {
        result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    }
    return result_types;
}

TableEntry *LogicalIndexScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalIndexScan::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalIndexScan::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalIndexScan::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "IndexScan: " << *base_table_ref_->table_entry_ptr_->GetTableName();
    ss << ", filter expression: " << index_filter_qualified_->Name();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
