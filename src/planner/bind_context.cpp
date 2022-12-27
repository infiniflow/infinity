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
BindContext::GetCTE(const String& name) const {
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
BindContext::IsViewBound(const String& view_name) const {

    if(bound_view_set_.contains(view_name)) {
        return true;
    }

    if(parent_) {
        return parent_->IsViewBound(view_name);
    }

    return false;
}

bool
BindContext::IsTableBound(const String& table_name) const {

    if(bound_table_set_.contains(table_name)) {
        return true;
    }

    if(parent_) {
        return parent_->IsTableBound(table_name);
    }

    return false;
}

int64_t
BindContext::GetNewLogicalNodeId() {
    return parent_ ? parent_->GetNewLogicalNodeId() : next_logical_node_id_ ++;
}

size_t
BindContext::GenerateBindingContextIndex() {
    return parent_ ? parent_->GenerateBindingContextIndex() : next_bind_context_index_ ++;
}

void
BindContext::AddBinding(const std::shared_ptr<Binding>& binding) {
    binding_by_name_.emplace(binding->table_name_, binding);
    for(auto& column_name: binding->column_names_) {
        auto iter = binding_names_by_column_.find(column_name);
        if(iter == binding_names_by_column_.end()) {
            binding_names_by_column_.emplace(column_name, Vector<String>());
            binding_names_by_column_[column_name].emplace_back(binding->table_name_);
        } else {
            iter->second.emplace_back(binding->table_name_);
        }
    }
}

void
BindContext::AddSubqueryBinding(const String& name,
                               const Vector<DataType>& column_types,
                               const Vector<String>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kSubquery, name, column_types, column_names);
    AddBinding(binding);
}

void
BindContext::AddCTEBinding(const String& name, const Vector<DataType>& column_types,
                           const Vector<String>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kCTE, name, column_types, column_names);
    AddBinding(binding);
}

void
BindContext::AddViewBinding(const String& name, const Vector<DataType>& column_types,
                            const Vector<String>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kView, name, column_types, column_names);
    AddBinding(binding);
}

void
BindContext::AddTableBinding(const String& name, std::shared_ptr<Table> table_ptr,
//                             int64_t logical_node_id,
//                             std::shared_ptr<LogicalNode> logical_node_ptr,
                             const Vector<DataType>& column_types,
                             const Vector<String>& column_names) {
    auto binding = Binding::MakeBinding(BindingType::kTable, name, std::move(table_ptr),
//                                        std::move(logical_node_ptr),
//                                        logical_node_id,
                                        column_types, column_names);
    AddBinding(binding);
    table_names_.emplace_back(name);
}

void
BindContext::AddLeftChild(const std::shared_ptr<BindContext>& left_child) {
    this->left_child_ = left_child;
    this->AddBindContext(left_child);
}

void
BindContext::AddRightChild(const std::shared_ptr<BindContext>& right_child) {
    this->right_child_ = right_child;
    this->AddBindContext(right_child);
}

void
BindContext::AddSubQueryChild(const std::shared_ptr<BindContext>& child) {
    this->subquery_children_.emplace_back(child);
    // TODO: need merge bind context?
}

void
BindContext::AddBindContext(const std::shared_ptr<BindContext>& other_ptr) {

    for(auto& name_binding_pair : other_ptr->binding_by_name_) {
        auto& binding_name = name_binding_pair.first;
        if(this->binding_by_name_.contains(binding_name)) {
            PlannerError("Table: " + binding_name + " has already been bound before.");
        }
        this->binding_by_name_.emplace(name_binding_pair);
    }

    for(auto& column_name_binding_pair: other_ptr->binding_names_by_column_) {
        auto& column_name = column_name_binding_pair.first;
        auto& bindings_names = column_name_binding_pair.second;

        if(this->binding_names_by_column_.contains(column_name)) {
            for(auto& binding_name: bindings_names) {
                this->binding_names_by_column_[column_name].emplace_back(binding_name);
            }
        } else {
            this->binding_names_by_column_.emplace(column_name, bindings_names);
        }
    }
}

std::shared_ptr<BaseExpression>
BindContext::ResolveColumnId(const ColumnIdentifier& column_identifier, int64_t depth) {

    std::shared_ptr<BaseExpression> bound_column_expr;

    const String& column_name_ref = *column_identifier.column_name_ptr_;

    if(column_identifier.table_name_ptr_ == nullptr) {
        // Not table name
        // Try to find the column in current bind context;
        if(binding_names_by_column_.contains(column_name_ref)) {
            // We find the table
            // TODO: What will happen, when different tables have the same column name?
            Vector<String>& binding_names = binding_names_by_column_[column_name_ref];
            if(binding_names.size() > 1) {
                PlannerError("Ambiguous column name: " + column_identifier.ToString());
            }

            String& binding_name = binding_names[0];
            auto binding = binding_by_name_[binding_name];

            if(binding->name2index_.contains(column_name_ref)) {
                int64_t column_id = binding->name2index_[column_name_ref];
                bound_column_expr = std::make_shared<ColumnExpression>(
                        binding->column_types_[column_id],
                        binding_name,
                        column_name_ref,
                        column_id,
                        depth);
                bound_column_expr->source_position_
                        = SourcePosition(binding_context_id_, ExprSourceType::kBinding);
                bound_column_expr->source_position_.binding_name_ = binding->table_name_;
            } else {
                // Found the binding, but the binding don't have the column, which should happen.
                PlannerError(column_identifier.ToString() + " doesn't exist.");
            }
        } else {
            // Table isn't found in current bind context, maybe its parent has it.
            bound_column_expr = nullptr;
        }
    } else {
        const String& table_name_ref = *column_identifier.table_name_ptr_;
        auto binding = binding_by_name_[table_name_ref];
        if(binding != nullptr) {
            if(binding->name2index_.contains(column_name_ref)) {
                // Find the table and column in the bind context.
                int64_t column_id = binding->name2index_[column_name_ref];
                bound_column_expr = std::make_shared<ColumnExpression>(
                        binding->column_types_[column_id],
                        table_name_ref,
                        column_name_ref,
                        column_id,
                        depth);
                bound_column_expr->source_position_
                        = SourcePosition(binding_context_id_, ExprSourceType::kBinding);
                bound_column_expr->source_position_.binding_name_ = binding->table_name_;
            } else {
                PlannerError(column_identifier.ToString() + " doesn't exist.");
            }
        } else {
            // Table isn't found in current bind context, maybe its parent has it.
            bound_column_expr = nullptr;
        }
    }

    if(bound_column_expr != nullptr) return bound_column_expr;

    if(parent_ != nullptr) {
        return parent_->ResolveColumnId(column_identifier, depth + 1);
    }

    PlannerError(column_identifier.ToString() + " isn't found.");
}

//void
//BindContext::AddChild(const std::shared_ptr<BindContext>& child) {
//    child->binding_context_id_ = GenerateBindingContextIndex();
//    children_.emplace_back(child);
//}

}