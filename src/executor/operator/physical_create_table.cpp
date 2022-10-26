//
// Created by JinHai on 2022/7/28.
//

#include "physical_create_table.h"
#include "main/infinity.h"
#include "common/utility/infinity_assert.h"
#include "storage/table_with_fix_row.h"

#include <utility>

namespace infinity {


PhysicalCreateTable::PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                         std::shared_ptr<TableDefinition> table_def_ptr,
                                         uint64_t id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, nullptr, nullptr, id),
      schema_name_(std::move(schema_name)),
    table_def_ptr_(std::move(table_def_ptr)) {

}

PhysicalCreateTable::PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                         const std::shared_ptr<PhysicalOperator>& input,
                                         uint64_t id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, input, nullptr, id),
      schema_name_(std::move(schema_name)) {

}

void
PhysicalCreateTable::Init() {

}

void
PhysicalCreateTable::Execute(std::shared_ptr<QueryContext>& query_context) {
//    ResponseError("Execute: Create table: " + table_def_ptr_->name());
    std::shared_ptr<Table> table_ptr = std::make_shared<FixedRowCountTable>(table_def_ptr_);
    Infinity::instance().catalog()->AddTable(*schema_name_, table_ptr);

    // Generate the result
    std::vector<ColumnDefinition> column_defs = {{"OK", 0, LogicalType(LogicalTypeId::kInteger), false, std::set<ConstrainType>()}};

    std::shared_ptr<TableDefinition> table_def_ptr
        = std::make_shared<TableDefinition>("Tables", column_defs, false);
    output_ = std::make_shared<FixedRowCountTable>(table_def_ptr);
}


}
