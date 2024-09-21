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

module logical_match_scan_base;

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import match_tensor_expression;
import default_values;
import logical_type;
import internal_types;
import third_party;
import explain_logical_plan;
import search_options;
import infinity_exception;
import status;
import logger;

namespace infinity {

LogicalMatchScanBase::LogicalMatchScanBase(u64 node_id,
                                           LogicalNodeType node_type,
                                           SharedPtr<BaseTableRef> base_table_ref,
                                           SharedPtr<BaseExpression> query_expression)
    : LogicalNode(node_id, node_type), base_table_ref_(base_table_ref), query_expression_(query_expression)

{
    //
}

Vector<ColumnBinding> LogicalMatchScanBase::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (SizeT col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalMatchScanBase::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    const SizeT column_count = base_table_ref_->column_names_->size();
    result_names->reserve(column_count + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalMatchScanBase::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    const SizeT column_count = base_table_ref_->column_names_->size();
    result_types->reserve(column_count + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(query_expression_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

TableEntry *LogicalMatchScanBase::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalMatchScanBase::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalMatchScanBase::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalMatchScanBase::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> ";
    }
    arrow_str += const_cast<LogicalMatchScanBase *>(this)->name();
    arrow_str += fmt::format(" ({})", this->node_id());
    ss << arrow_str << std::endl;

    // Table alias and name
    String table_name = String(space, ' ');
    table_name += " - table name: ";
    table_name += this->TableAlias();
    table_name += "(";
    table_name += *base_table_ref_->table_entry_ptr_->GetDBName();
    table_name += ".";
    table_name += *base_table_ref_->table_entry_ptr_->GetTableName();
    table_name += ")";
    ss << table_name << std::endl;

    // Table index
    String table_index = String(space, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(this->TableIndex());
    ss << table_index << std::endl;

    String match_info = String(space, ' ');
    match_info += " - match tensor expression: " + query_expression_->ToString();
    ss << match_info << std::endl;

    // filter expression
    if (filter_expression_.get() != nullptr) {
        ss << String(space, ' ');
        ss << " - filter: ";
        String filter_s;
        ExplainLogicalPlan::Explain(filter_expression_.get(), filter_s);
        ss << filter_s << '\n';
        if (common_query_filter_) {
            ss << String(space, ' ');
            ss << " - filter with index: ";
            if (common_query_filter_->index_filter_) {
                String filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->index_filter_.get(), filter_str);
                ss << filter_str << '\n';
            } else {
                ss << "None\n";
            }
            ss << String(space, ' ');
            ss << " - filter without index: ";
            if (common_query_filter_->leftover_filter_) {
                String filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->leftover_filter_.get(), filter_str);
                ss << filter_str << '\n';
            } else {
                ss << "None\n";
            }
        }
    }

    // Output columns
    String output_columns = String(space, ' ');
    output_columns += " - output columns: [";
    SharedPtr<Vector<String>> output_names = this->GetOutputNames();
    SizeT column_count = output_names->size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        if (idx != 0)
            output_columns += ", ";
        output_columns += output_names->at(idx);
    }
    output_columns += "]";
    ss << output_columns << std::endl;

    return ss.str();
}

} // namespace infinity
