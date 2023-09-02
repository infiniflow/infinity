//
// Created by JinHai on 2022/7/28.
//

#include "physical_limit.h"
#include "expression/value_expression.h"

namespace infinity {

void
PhysicalLimit::Init() {

}

void
PhysicalLimit::Execute(QueryContext* query_context) {

    // output table definition is same as input
    input_table_ = left_->output();
    ExecutorAssert(input_table_ != nullptr, "No input");

    ExecutorAssert(limit_expr_->type() == ExpressionType::kValue,
                   "Currently, only support constant limit expression");

    i64 limit = (std::static_pointer_cast<ValueExpression>(limit_expr_))->GetValue().value_.big_int;
    ExecutorAssert(limit > 0, "Limit should be larger than 0")

    i64 offset = 0;
    if(offset_expr_ != nullptr) {
        ExecutorAssert(offset_expr_->type() == ExpressionType::kValue,
                       "Currently, only support constant limit expression");
        offset = (std::static_pointer_cast<ValueExpression>(offset_expr_))->GetValue().value_.big_int;
        ExecutorAssert(offset >= 0 && offset < input_table_->row_count(),
                       "Offset should be larger or equal than 0 and less than row number")
    }

    output_ = GetLimitOutput(input_table_, limit, offset);
}

SharedPtr<Table>
PhysicalLimit::GetLimitOutput(const SharedPtr<Table>& input_table, i64 limit, i64 offset) {
    SizeT start_block = 0;
    SizeT start_row_id = 0;
    SizeT end_block = 0;
    SizeT end_row_id = 0;

    if(offset == 0) {
        if(limit >= input_table->row_count()) {
            return input_table;
        } else {
            start_block = 0;
            start_row_id = 0;
            SizeT block_count = input_table->DataBlockCount();
            i64 total_row_count = limit;
            for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
                SizeT block_row_count = input_table->GetDataBlockById(block_id)->row_count();
                if(total_row_count > block_row_count) {
                    total_row_count -= block_row_count;
                } else {
                    end_block = block_id;
                    end_row_id = total_row_count;
                    break;
                }
            }
        }
    } else {
        i64 total_row_count = offset;
        SizeT block_count = input_table->DataBlockCount();
        SizeT rest_row_count = 0;
        for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
            SizeT block_row_count = input_table->GetDataBlockById(block_id)->row_count();
            if(total_row_count >= block_row_count) {
                total_row_count -= block_row_count;
            } else if (total_row_count < block_row_count){
                start_block = block_id;
                start_row_id = total_row_count;
                rest_row_count = block_row_count - total_row_count;
                break;
            }
        }

        total_row_count = limit;
        if(total_row_count <= rest_row_count) {
            end_block = start_block;
            end_row_id = total_row_count;
        } else {
            total_row_count -= rest_row_count;
            for(SizeT block_id = start_block + 1; block_id < block_count; ++ block_id) {
                SizeT block_row_count = input_table->GetDataBlockById(block_id)->row_count();
                if(total_row_count > block_row_count) {
                    total_row_count -= block_row_count;
                } else {
                    end_block = block_id;
                    end_row_id = total_row_count;
                    break;
                }
            }
        }
    }

    // Copy from input table to output table
    SizeT column_count = input_table->ColumnCount();
    Vector<SharedPtr<DataType>> types;
    types.reserve(column_count);
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<DataType> col_type = input_table->GetColumnTypeById(idx);
        types.emplace_back(col_type);

        String col_name = input_table->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, col_type,col_name, HashSet<ConstraintType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"),
                                                   MakeShared<String>("limit"),
                                                   columns);
    SharedPtr<Table> output_table = Table::Make(table_def, TableType::kIntermediate);

    const Vector<SharedPtr<DataBlock>>& input_datablocks = input_table->data_blocks_;

    for(SizeT block_id = start_block; block_id <= end_block; ++ block_id) {
        SizeT input_start_offset = start_row_id;
        SizeT input_end_offset;
        if(end_block == block_id) {
            input_end_offset = end_row_id;
        } else {
            // current input block isn't the last one.
            input_end_offset = input_datablocks[block_id]->row_count();
        }

        SharedPtr<DataBlock> output_datablock = DataBlock::Make();
        output_datablock->Init(input_datablocks[block_id], input_start_offset, input_end_offset);
        output_table->Append(output_datablock);

        start_row_id = 0;
    }
    return output_table;
}

}
