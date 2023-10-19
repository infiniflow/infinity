//
// Created by JinHai on 2022/9/23.
//

module;

import stl;
import parser;
import infinity_assert;
import infinity_exception;
import third_party;

module column_identifer;

namespace infinity {

ColumnIdentifier ColumnIdentifier::MakeColumnIdentifier(QueryContext *query_context, const ColumnExpr &expr) {
    if (expr.star_) {
        Error<PlannerException>("Star expression should be unfolded before.", __FILE_NAME__, __LINE__);
    }

    SharedPtr<String> db_name_ptr = nullptr;
    SharedPtr<String> schema_name_ptr = nullptr;
    SharedPtr<String> table_name_ptr = nullptr;
    SharedPtr<String> column_name_ptr = nullptr;

    i64 name_count = expr.names_.size();
    Assert<PlannerException>(name_count <= 4 && name_count > 0, "Star expression should be unfolded before.", __FILE_NAME__, __LINE__);
    --name_count;
    column_name_ptr = MakeShared<String>(expr.names_[name_count]);
    --name_count;
    if (name_count >= 0) {
        table_name_ptr = MakeShared<String>(expr.names_[name_count]);
        --name_count;
        if (name_count >= 0) {
            schema_name_ptr = MakeShared<String>(expr.names_[name_count]);
            --name_count;
            if (name_count >= 0) {
                db_name_ptr = MakeShared<String>(expr.names_[name_count]);
            }
        }
    }

    SharedPtr<String> alias_name_ptr = nullptr;
    if (!expr.alias_.empty()) {
        alias_name_ptr = MakeShared<String>(expr.alias_);
    }
    return ColumnIdentifier(db_name_ptr, schema_name_ptr, table_name_ptr, column_name_ptr, alias_name_ptr);
}

ColumnIdentifier::ColumnIdentifier(SharedPtr<String> db_name,
                                   SharedPtr<String> schema_name,
                                   SharedPtr<String> table_name,
                                   SharedPtr<String> column_name,
                                   SharedPtr<String> alias_name)
    : db_name_ptr_(Move(db_name)), schema_name_ptr_(Move(schema_name)), column_name_ptr_(Move(column_name)), table_name_ptr_(Move(table_name)),
      alias_name_ptr_(Move(alias_name)) {}

String ColumnIdentifier::ToString() const {
    if (table_name_ptr_.get() != nullptr)
        return Format("{}.{}", *table_name_ptr_, *column_name_ptr_);
    else
        return *column_name_ptr_;
}

bool ColumnIdentifier::operator==(const ColumnIdentifier &other) const {
    if (this == &other)
        return true;
    if (IsEqual(*column_name_ptr_, *other.column_name_ptr_)) {
        return false;
    }
    if (table_name_ptr_.get() != nullptr && other.table_name_ptr_.get() != nullptr) {
        return IsEqual(*table_name_ptr_, *other.table_name_ptr_);
    }

    if (table_name_ptr_.get() == nullptr && other.table_name_ptr_.get() == nullptr) {
        return true;
    }

    return false;
}

} // namespace infinity