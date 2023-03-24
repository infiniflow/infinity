//
// Created by JinHai on 2022/7/28.
//

#include "physical_create_table.h"
#include "main/infinity.h"
#include "common/utility/infinity_assert.h"

#include <utility>

namespace infinity {


PhysicalCreateTable::PhysicalCreateTable(SharedPtr<String> schema_name,
                                         SharedPtr<TableDef> table_def_ptr,
                                         ConflictType conflict_type,
                                         u64 table_index,
                                         u64 id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, nullptr, nullptr, id),
      schema_name_(std::move(schema_name)),
      conflict_type_(conflict_type),
      table_index_(table_index),
      table_def_ptr_(std::move(table_def_ptr)) {

}

PhysicalCreateTable::PhysicalCreateTable(SharedPtr<String> schema_name,
                                         const SharedPtr<PhysicalOperator>& input,
                                         ConflictType conflict_type,
                                         u64 table_index,
                                         u64 id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, input, nullptr, id),
      schema_name_(std::move(schema_name)),
      conflict_type_(conflict_type),
      table_index_(table_index) {

}

void
PhysicalCreateTable::Init() {

}

void
PhysicalCreateTable::Execute(SharedPtr<QueryContext>& query_context) {
//    ResponseError("Execute: Create table: " + table_def_ptr_->name());
    SharedPtr<Table> table_ptr = MakeShared<Table>(table_def_ptr_, TableType::kDataTable);
    Infinity::instance().catalog()->CreateTable(*schema_name_, table_ptr, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
        = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

}
