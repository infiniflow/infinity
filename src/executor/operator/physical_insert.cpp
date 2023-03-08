//
// Created by JinHai on 2022/7/28.
//

#include "storage/table_with_fix_row.h"
#include "physical_insert.h"
#include "expression/value_expression.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
PhysicalInsert::Init() {

}

void
PhysicalInsert::Execute(SharedPtr<QueryContext>& query_context) {

    // TODO: execute insert into table;
    Vector<DataType> output_types;
    SizeT value_count = value_list_.size();
    for(SizeT idx = 0; idx < value_count; ++ idx) {
        if (value_list_[idx]->type() != ExpressionType::kValue) {
            ExecutorError("Not value expression.");
        }
        output_types.emplace_back(value_list_[idx]->Type());
    }

    SharedPtr<DataBlock> output_block = MakeShared<DataBlock>();
    output_block->Init(output_types);
    for(SizeT idx = 0; idx < value_count; ++ idx) {
        SharedPtr<ValueExpression> value_expr_ptr = std::static_pointer_cast<ValueExpression>(value_list_[idx]);
        value_expr_ptr->AppendToChunk(output_block->column_vectors[idx]);
    }
    // Finalize the output block row count.
    output_block->Finalize();

    table_ptr_->Append(output_block);

    // Generate the result table
    Vector<SharedPtr<ColumnDef>> column_defs;
    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(table_def_ptr, TableType::kResult);
}

}

