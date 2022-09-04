//
// Created by JinHai on 2022/8/7.
//

#include "bind_context.h"
#include "common/utility/infinity_assert.h"
#include "storage/table.h"

namespace infinity {

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