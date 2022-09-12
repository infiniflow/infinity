//
// Created by JinHai on 2022/8/7.
//

#include "bind_context.h"
#include "common/utility/infinity_assert.h"
#include "storage/table.h"

namespace infinity {

std::shared_ptr<CommonTableExpressionInfo>
BindContext::GetCTE(const std::string& name) const {
    auto entry = CTE_map_.find(name);
    if(entry != CTE_map_.end()) {
        return entry->second;
    }

    if(parent_) {
        return parent_->GetCTE(name);
    }
    return nullptr;
}

bool
BindContext::IsCTEBound(const std::shared_ptr<CommonTableExpressionInfo>& cte) const {

    if(bound_cte_set_.contains(cte)) {
        return true;
    }

    if(parent_) {
        return parent_->IsCTEBound(cte);
    }

    return false;
}

bool
BindContext::IsViewBound(const std::string& view_name) const {

    if(bound_view_set_.contains(view_name)) {
        return true;
    }

    if(parent_) {
        return parent_->IsViewBound(view_name);
    }

    return false;
}

bool
BindContext::IsTableBound(const std::string& table_name) const {

    if(bound_table_set_.contains(table_name)) {
        return true;
    }

    if(parent_) {
        return parent_->IsTableBound(table_name);
    }

    return false;
}

int64_t
BindContext::GetNewTableIndex() {
    return parent_ ? parent_->GetNewTableIndex() : next_table_index_ ++;
}

int64_t
BindContext::GetNewLogicalNodeId() {
    return parent_ ? parent_->GetNewLogicalNodeId() : next_logical_node_id_ ++;
}

void
BindContext::AddSubqueryBinding(const std::string& name, int64_t table_index,
                               const std::vector<LogicalType>& column_types,
                               const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kSubquery, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_[name] = binding;
}

void
BindContext::AddCTEBinding(const std::string& name, int64_t table_index, const std::vector<LogicalType>& column_types,
                           const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kCTE, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_[name] = binding;
}

void
BindContext::AddViewBinding(const std::string& name, int64_t table_index, const std::vector<LogicalType>& column_types,
                            const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kView, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_[name] = binding;
}

void
BindContext::AddTableBinding(const std::string& name, int64_t table_index, int64_t logical_node_id,
                             std::shared_ptr<LogicalNode> logical_node_ptr,
                             const std::vector<LogicalType>& column_types,
                             const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kTable, name, table_index, logical_node_id, std::move(logical_node_ptr), column_types, column_names);
    bindings_by_name_[name] = binding;
}

// !!! TODO: Below need to be refactored !!!

std::shared_ptr<BaseExpression>
BindContext::ResolveColumnIdentifier(const ColumnIdentifier& column_identifier) {
    PlannerError("Not implement: BindContext::resolve_column_identifier");
    return std::shared_ptr<BaseExpression>();
}

void
BindContext::AddTable(const std::shared_ptr<Table>& table_ptr) {
    std::string table_name = table_ptr->table_def()->name();
    if(tables_by_name_.contains(table_name)) {
        PlannerError("Duplicate table when binding.")
    }

    // Add table into binding table arrays
    tables_.emplace_back(table_ptr);

    // Build a binding table index by table name
    tables_by_name_[table_name] = table_ptr;

}

}