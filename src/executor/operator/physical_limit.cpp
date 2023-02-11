//
// Created by JinHai on 2022/7/28.
//

#include "physical_limit.h"
#include "expression/value_expression.h"

namespace infinity {

void
PhysicalLimit::Init() {
    // output table definition is same as input
    ExecutorAssert(left()->outputs().size() == 1, "Input table count isn't matched.");

    for(const auto& input_table: left()->outputs()) {
        input_table_ = input_table.second;
        input_table_index_ = input_table.first;
    }

    SizeT column_count = input_table_->ColumnCount();
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = input_table_->GetColumnTypeById(idx);
        String col_name = input_table_->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make("limit", columns, false);

    outputs_[input_table_index_] = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalLimit::Execute(SharedPtr<QueryContext>& query_context) {

    ExecutorAssert(limit_expr_->type() == ExpressionType::kValue,
                   "Currently, only support constant limit expression");

    i64 offset = 0;

    if(offset_expr_ != nullptr) {
        ExecutorAssert(offset_expr_->type() == ExpressionType::kValue,
                       "Currently, only support constant limit expression");
        offset = (std::static_pointer_cast<ValueExpression>(offset_expr_))->GetValue().value_.big_int;
    }


    i64 limit = (std::static_pointer_cast<ValueExpression>(limit_expr_))->GetValue().value_.big_int;

    ExecutorAssert(limit > 0, "Limit should be larger than 0")
    ExecutorAssert(offset >= 0 && offset < input_table_->row_count(),
                   "Offset should be larger or equal than 0 and less than row number")

    outputs_[input_table_index_] = GetLimitOutput(input_table_, limit, offset);
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
    Vector<DataType> types;
    types.reserve(column_count);
    Vector<SharedPtr<ColumnDef>> columns;
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        DataType col_type = input_table->GetColumnTypeById(idx);
        types.emplace_back(col_type);

        String col_name = input_table->GetColumnNameById(idx);

        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
        columns.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def = TableDef::Make("limit", columns, false);
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
