//
// Created by JinHai on 2022/9/23.
//

#include "column_identifier.h"

namespace infinity {

ColumnIdentifier
ColumnIdentifier::MakeColumnIdentifier(SharedPtr<QueryContext>& query_context,
                                       const hsql::Expr& expr) {
    SharedPtr<String> db_name_ptr = nullptr;
    SharedPtr<String> schema_name_ptr = nullptr;
    SharedPtr<String> table_name_ptr = nullptr;
    SharedPtr<String> column_name_ptr = MakeShared<String>(expr.name);

    SharedPtr<String> alias_name_ptr = nullptr;

    // TODO: db name
    db_name_ptr = MakeShared<String>();

    // Check schema name;
    // TODO: currently parser don't support schema name in expression
    schema_name_ptr = MakeShared<String>(query_context->schema_name());

    if(expr.table != nullptr) {
        table_name_ptr = MakeShared<String>(expr.table);
    }
    if(expr.alias != nullptr) {
        alias_name_ptr = MakeShared<String>(expr.alias);
    }
    return ColumnIdentifier(db_name_ptr, schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}

ColumnIdentifier
ColumnIdentifier::MakeColumnIdentifier(SharedPtr<QueryContext>& query_context,
                                       const SharedPtr<ParsedColumnExpression>& parsed_expr) {
    SharedPtr<String> db_name_ptr = MakeShared<String>(parsed_expr->db_name_);
    SharedPtr<String> schema_name_ptr = MakeShared<String>(parsed_expr->schema_name_);
    SharedPtr<String> table_name_ptr = MakeShared<String>(parsed_expr->table_name_);
    SharedPtr<String> column_name_ptr = MakeShared<String>(parsed_expr->column_name_);
    SharedPtr<String> alias_ptr = MakeShared<String>(parsed_expr->alias_);

    SharedPtr<String> alias_name_ptr = nullptr;

    return ColumnIdentifier(db_name_ptr, schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}


ColumnIdentifier::ColumnIdentifier(SharedPtr<String> db_name,
                                   SharedPtr<String> schema_name,
                                   SharedPtr<String> table_name,
                                   SharedPtr<String> column_name,
                                   SharedPtr<String> alias_name)
                                   : db_name_ptr_(std::move(db_name)),
                                   schema_name_ptr_(std::move(schema_name)),
                                   column_name_ptr_(std::move(column_name)),
                                   table_name_ptr_(std::move(table_name)),
                                   alias_name_ptr_(std::move(alias_name))
{}

String
ColumnIdentifier::ToString() const {
    if(table_name_ptr_ != nullptr) return *table_name_ptr_ + "." + *column_name_ptr_;
    else return *column_name_ptr_;
}

bool
ColumnIdentifier::operator==(const ColumnIdentifier& other) const {
    if(this == &other) return true;
    if(*column_name_ptr_ != *other.column_name_ptr_) {
        return false;
    }
    if(table_name_ptr_ != nullptr && other.table_name_ptr_ != nullptr) {
        return *table_name_ptr_ == *other.table_name_ptr_;
    }

    if(table_name_ptr_ == nullptr && other.table_name_ptr_ == nullptr) {
        return true;
    }

    return false;
}

}