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

#include <sstream>

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import knn_expression;
import parser;
import default_values;

module logical_knn_scan;

namespace infinity {

LogicalKnnScan::LogicalKnnScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref)
    : LogicalNode(node_id, LogicalNodeType::kKnnScan), base_table_ref_(Move(base_table_ref)) {}

Vector<ColumnBinding> LogicalKnnScan::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT column_count = base_table_ref_->column_ids_.size();
    result.reserve(column_count + 1);
    for (SizeT i = 0; i < column_count; ++i) {
        result.emplace_back(base_table_ref_->table_index_, base_table_ref_->column_ids_[i]);
    }
    result.emplace_back(knn_table_index_, 0);
    return result;
}

SharedPtr<Vector<String>> LogicalKnnScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    SizeT column_count = base_table_ref_->column_names_->size();
    result_names->reserve(column_count);
    for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        const auto &column_name = base_table_ref_->column_names_->at(col_idx);
        result_names->emplace_back(column_name);
    }
    result_names->emplace_back(knn_expression_->Name());
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalKnnScan::GetOutputTypes() const {
    Vector<SharedPtr<DataType>> result_types = *base_table_ref_->column_types_;
    result_types.reserve(result_types.size() + 2);
    result_types.emplace_back(MakeShared<DataType>(knn_expression_->Type()));
    result_types.emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return MakeShared<Vector<SharedPtr<DataType>>>(result_types);
}

TableEntry *LogicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalKnnScan::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalKnnScan::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "KnnScan: " << *base_table_ref_->table_entry_ptr_->GetTableName() << ", on: ";
    SizeT column_count = base_table_ref_->column_names_->size();
    for (SizeT i = 0; i < column_count - 1; ++i) {
        ss << base_table_ref_->column_names_->at(i) << " ";
    }
    ss << base_table_ref_->column_names_->back();
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
