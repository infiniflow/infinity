//
// Created by JinHai on 2022/7/28.
//

#include "storage/table_with_fix_row.h"
#include "physical_insert.h"
#include "expression/value_expression.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
PhysicalInsert::Execute() {

    // TODO: execute insert into table;
    std::vector<LogicalType> chunk_types;
    uint64_t value_count = value_list_.size();
    for(uint64_t idx = 0; idx < value_count; ++ idx) {
        if (value_list_[idx]->type() != ExpressionType::kValue) {
            ExecutorError("Not value expression.");
        }
        chunk_types.emplace_back(value_list_[idx]->DataType());
    }

    TransientBlock transient_block(chunk_types);
    for(uint64_t idx = 0; idx < value_count; ++ idx) {
        std::shared_ptr<ValueExpression> value_expr_ptr = std::static_pointer_cast<ValueExpression>(value_list_[idx]);
        value_expr_ptr->AppendToChunk(transient_block.chunks_[idx]);
    }
    transient_block.row_count_ += 1;

    table_ptr_->Append(transient_block);

    // Generate the result
    std::vector<ColumnDefinition> column_defs;
    std::shared_ptr<TableDefinition> table_def_ptr
            = std::make_shared<TableDefinition>("Tables", column_defs, false);
    output_ = std::make_shared<FixedRowCountTable>(table_def_ptr);
}

}

