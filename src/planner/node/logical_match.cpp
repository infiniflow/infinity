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

module logical_match;

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import match_expression;

import default_values;
import third_party;

import logical_type;
import internal_types;
import explain_logical_plan;

namespace infinity {

LogicalMatch::LogicalMatch(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<MatchExpression> match_expr)
    : LogicalNode(node_id, LogicalNodeType::kMatch), base_table_ref_(base_table_ref), match_expr_(std::move(match_expr)) {}

Vector<ColumnBinding> LogicalMatch::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (SizeT col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalMatch::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    SizeT column_count = base_table_ref_->column_names_->size();
    result_names->reserve(column_count + 2);
    for (auto &name : *base_table_ref_->column_names_)
        result_names->emplace_back(name);
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalMatch::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    SizeT column_count = base_table_ref_->column_names_->size();
    result_types->reserve(column_count + 2);
    for (auto &type : *base_table_ref_->column_types_)
        result_types->emplace_back(type);
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kFloat));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

TableEntry *LogicalMatch::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String LogicalMatch::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalMatch::TableIndex() const { return base_table_ref_->table_index_; }

String LogicalMatch::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> LogicalMatch ";
    } else {
        arrow_str = "LogicalMatch ";
    }
    arrow_str += fmt::format("({})", this->node_id());
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
    match_info += " - match info: " + match_expr_->ToString();
    ss << match_info << std::endl;

    // filter expression
    if (filter_expression_.get() != nullptr) {
        ss << String(space, ' ');
        ss << " - filter: ";
        String filter_str;
        ExplainLogicalPlan::Explain(filter_expression_.get(), filter_str);
        ss << filter_str << '\n';
        if (common_query_filter_) {
            ss << String(space, ' ');
            ss << " - filter for secondary index: ";
            if (common_query_filter_->secondary_index_filter_qualified_) {
                String filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->secondary_index_filter_qualified_.get(), filter_str);
                ss << filter_str << '\n';
            } else {
                ss << "None\n";
            }
            ss << String(space, ' ');
            ss << " - filter except secondary index: ";
            if (common_query_filter_->filter_leftover_) {
                String filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->filter_leftover_.get(), filter_str);
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
