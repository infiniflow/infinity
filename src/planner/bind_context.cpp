//
// Created by JinHai on 2022/8/7.
//

#include "bind_context.h"
#include "common/utility/infinity_assert.h"
#include "expression/column_expression.h"
#include "storage/table.h"

namespace infinity {

BindContext::~BindContext() {
    Destroy();
}

void
BindContext::Destroy() {
    // TODO: Bind context need to release the resource carefully.
    parent_ = nullptr;
}

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

int64_t
BindContext::GenerateBindingContextIndex() {
    return parent_ ? parent_->GenerateBindingContextIndex() : next_bind_context_index_ ++;
}

void
BindContext::AddSubqueryBinding(const std::string& name, int64_t table_index,
                               const std::vector<LogicalType>& column_types,
                               const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kSubquery, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_.emplace(name, binding);
    bindings_by_table_index_.emplace(table_index, binding);
    for(auto& column_name: column_names) {
        bindings_by_column_.emplace(column_name, binding);
    }

    bindings_.emplace_back(binding);
}

void
BindContext::AddCTEBinding(const std::string& name, int64_t table_index, const std::vector<LogicalType>& column_types,
                           const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kCTE, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_.emplace(name, binding);
    bindings_by_table_index_.emplace(table_index, binding);
    for(auto& column_name: column_names) {
        bindings_by_column_.emplace(column_name, binding);
    }

    bindings_.emplace_back(binding);
}

void
BindContext::AddViewBinding(const std::string& name, int64_t table_index, const std::vector<LogicalType>& column_types,
                            const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kView, name, table_index, -1, nullptr, column_types, column_names);
    bindings_by_name_.emplace(name, binding);
    bindings_by_table_index_.emplace(table_index, binding);
    for(auto& column_name: column_names) {
        bindings_by_column_.emplace(column_name, binding);
    }

    bindings_.emplace_back(binding);
}

void
BindContext::AddTableBinding(const std::string& name, int64_t table_index, int64_t logical_node_id,
                             std::shared_ptr<LogicalNode> logical_node_ptr,
                             const std::vector<LogicalType>& column_types,
                             const std::vector<std::string>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kTable, name, table_index, logical_node_id, std::move(logical_node_ptr), column_types, column_names);
    bindings_by_name_.emplace(name, binding);
    bindings_by_table_index_.emplace(table_index, binding);
    for(auto& column_name: column_names) {
        bindings_by_column_.emplace(column_name, binding);
    }

    bindings_.emplace_back(binding);
}

void
BindContext::AddBindContext(const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Not implement: BindContext::AddBindContext");
}

std::shared_ptr<BaseExpression>
BindContext::ResolveColumnIdentifier(const ColumnIdentifier& column_identifier, int64_t depth) {
    const std::string& table_name_ref = *column_identifier.table_name_ptr_;
    const std::string& column_name_ref = *column_identifier.column_name_ptr_;

    if(column_identifier.table_name_ptr_ != nullptr) {
        auto binding = bindings_by_name_[table_name_ref];
        if(binding != nullptr) {
            if(binding->name2index_.contains(column_name_ref)) {
                // Find the table and column in the bind context.
                int64_t column_id = binding->name2index_[column_name_ref];
                std::shared_ptr<ColumnExpression> column_expr
                    = std::make_shared<ColumnExpression>(
                            binding->column_types_[column_id],
                            table_name_ref,
                            binding->table_index_,
                            column_name_ref,
                            column_id,
                            depth);
                return column_expr;
            } else {
                PlannerError(column_identifier.ToString() + " doesn't exist.");
            }
        } else {
            // Table isn't found in this bind context, maybe its parent has it.
            // Following code will proceed this case.
        }
    } else {
        // Not table name
        // Try to find the column in current bind context;
        if(bindings_by_column_.contains(column_name_ref)) {
            // We find the table
            // TODO: What will happen, when different tables have the same column name?

            auto binding = bindings_by_column_[column_name_ref];
            if(binding->name2index_.contains(column_name_ref)) {
                int64_t column_id = binding->name2index_[column_name_ref];
                std::shared_ptr<ColumnExpression> column_expr
                        = std::make_shared<ColumnExpression>(
                                binding->column_types_[column_id],
                                table_name_ref,
                                binding->table_index_,
                                column_name_ref,
                                column_id,
                                depth);
                return column_expr;
            } else {
                // Found the binding, but the binding don't have the column, which should happen.
                PlannerError(column_identifier.ToString() + " doesn't exist.");
            }
        } else {
            // Bind isn't found in this bind context, try to find in parent_
        }
    }

    if(parent_ != nullptr) {
        return parent_->ResolveColumnIdentifier(column_identifier, depth + 1);
    }

    PlannerError(column_identifier.ToString() + " isn't found.");
}

void
BindContext::AddChild(const std::shared_ptr<BindContext>& child) {
    child->id_ = GenerateBindingContextIndex();
    children_.emplace_back(child);
}

// !!! TODO: Below need to be refactored !!!


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