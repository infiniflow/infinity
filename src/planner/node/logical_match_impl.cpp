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

module infinity_core:logical_match.impl;

import :logical_match;
import :base_table_ref;
import :column_binding;
import :logical_node_type;
import :match_expression;
import :default_values;
import :meta_info;
import :explain_logical_plan;

import std;
import third_party;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

LogicalMatch::LogicalMatch(u64 node_id, std::shared_ptr<BaseTableRef> base_table_ref, std::shared_ptr<MatchExpression> match_expr)
    : LogicalNode(node_id, LogicalNodeType::kMatch), base_table_ref_(base_table_ref), match_expr_(std::move(match_expr)) {}

std::vector<ColumnBinding> LogicalMatch::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    auto &column_ids = base_table_ref_->column_ids_;
    result.reserve(column_ids.size());
    for (size_t col_id : column_ids) {
        result.emplace_back(base_table_ref_->table_index_, col_id);
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalMatch::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result_names = std::make_shared<std::vector<std::string>>();
    size_t column_count = base_table_ref_->column_names_->size();
    result_names->reserve(column_count + 2);
    for (auto &name : *base_table_ref_->column_names_)
        result_names->emplace_back(name);
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalMatch::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t column_count = base_table_ref_->column_names_->size();
    result_types->reserve(column_count + 2);
    for (auto &type : *base_table_ref_->column_types_)
        result_types->emplace_back(type);
    result_types->emplace_back(std::make_shared<DataType>(LogicalType::kFloat));
    result_types->emplace_back(std::make_shared<DataType>(LogicalType::kRowID));
    return result_types;
}

TableInfo *LogicalMatch::table_info() const { return base_table_ref_->table_info_.get(); }

std::string LogicalMatch::TableAlias() const { return base_table_ref_->alias_; }

u64 LogicalMatch::TableIndex() const { return base_table_ref_->table_index_; }

std::string LogicalMatch::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space != 0) {
        arrow_str = std::string(space - 2, ' ');
        arrow_str += "-> LogicalMatch ";
    } else {
        arrow_str = "LogicalMatch ";
    }
    arrow_str += fmt::format("({})", this->node_id());
    ss << arrow_str << std::endl;

    // Table alias and name
    std::string table_name = std::string(space, ' ');
    table_name += " - table name: ";
    table_name += this->TableAlias();
    table_name += "(";
    table_name += *base_table_ref_->table_info_->db_name_;
    table_name += ".";
    table_name += *base_table_ref_->table_info_->table_name_;
    table_name += ")";
    ss << table_name << std::endl;

    // Table index
    std::string table_index = std::string(space, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(this->TableIndex());
    ss << table_index << std::endl;

    std::string match_info = std::string(space, ' ');
    match_info += " - match info: " + match_expr_->ToString();
    ss << match_info << std::endl;

    // filter expression
    if (filter_expression_.get() != nullptr) {
        ss << std::string(space, ' ');
        ss << " - filter: ";
        std::string filter_str;
        ExplainLogicalPlan::Explain(filter_expression_.get(), filter_str);
        ss << filter_str << '\n';
        if (common_query_filter_) {
            ss << std::string(space, ' ');
            ss << " - filter with index: ";
            if (common_query_filter_->index_filter_) {
                std::string filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->index_filter_.get(), filter_str);
                ss << filter_str << '\n';
            } else {
                ss << "None\n";
            }
            ss << std::string(space, ' ');
            ss << " - filter without index: ";
            if (common_query_filter_->leftover_filter_) {
                std::string filter_str;
                ExplainLogicalPlan::Explain(common_query_filter_->leftover_filter_.get(), filter_str);
                ss << filter_str << '\n';
            } else {
                ss << "None\n";
            }
        }
    }

    // Output columns
    std::string output_columns = std::string(space, ' ');
    output_columns += " - output columns: [";
    std::shared_ptr<std::vector<std::string>> output_names = this->GetOutputNames();
    size_t column_count = output_names->size();
    for (size_t idx = 0; idx < column_count; ++idx) {
        if (idx != 0)
            output_columns += ", ";
        output_columns += output_names->at(idx);
    }
    output_columns += "]";
    ss << output_columns << std::endl;

    return ss.str();
}

} // namespace infinity
