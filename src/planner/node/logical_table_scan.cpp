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

import stl;
#include <sstream>
import column_binding;
import parser;
import base_expression;
import logical_node_type;
import base_table_ref;

module logical_table_scan;

namespace infinity {

LogicalTableScan::LogicalTableScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, bool add_row_id)
    : LogicalNode(node_id, LogicalNodeType::kTableScan), base_table_ref_(Move(base_table_ref)), add_row_id_(add_row_id) {}

Vector<ColumnBinding> LogicalTableScan::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (SizeT col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalTableScan::GetOutputNames() const { return base_table_ref_->column_names_; }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalTableScan::GetOutputTypes() const { return base_table_ref_->column_types_; }

TableCollectionEntry *LogicalTableScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalTableScan::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalTableScan::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalTableScan::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "TableScan: " << *base_table_ref_->table_entry_ptr_->table_collection_name_ << ", on: ";
    SizeT column_count = base_table_ref_->column_names_->size();
    for (SizeT i = 0; i < column_count - 1; ++i) {
        ss << base_table_ref_->column_names_->at(i) << " ";
    }
    ss << base_table_ref_->column_names_->back();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
