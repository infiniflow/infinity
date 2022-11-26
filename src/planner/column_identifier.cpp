//
// Created by JinHai on 2022/9/23.
//

#include "column_identifier.h"

namespace infinity {

ColumnIdentifier
ColumnIdentifier::MakeColumnIdentifier(std::shared_ptr<QueryContext>& query_context, const hsql::Expr &expr) {
    std::shared_ptr<std::string> column_name_ptr = std::make_shared<std::string>(expr.name);
    std::shared_ptr<std::string> schema_name_ptr = nullptr;
    std::shared_ptr<std::string> table_name_ptr = nullptr;
    std::shared_ptr<std::string> alias_name_ptr = nullptr;

    // Check schema name;
    // TODO: currently parser don't support schema name in expression
    schema_name_ptr = std::make_shared<std::string>(query_context->schema_name());

    if(expr.table != nullptr) {
        table_name_ptr = std::make_shared<std::string>(expr.table);
    }
    if(expr.alias != nullptr) {
        alias_name_ptr = std::make_shared<std::string>(expr.alias);
    }
    return ColumnIdentifier(schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}


ColumnIdentifier::ColumnIdentifier(std::shared_ptr<std::string> schema_name,
                                   std::shared_ptr<std::string> table_name,
                                   std::shared_ptr<std::string> column_name,
                                   std::shared_ptr<std::string> alias_name)
                                   : schema_name_ptr_(std::move(schema_name)),
                                   column_name_ptr_(std::move(column_name)),
                                   table_name_ptr_(std::move(table_name)),
                                   alias_name_ptr_(std::move(alias_name))
{}

std::string
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