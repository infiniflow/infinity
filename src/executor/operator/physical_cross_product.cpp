//
// Created by JinHai on 2022/7/28.
//

#include "physical_cross_product.h"

namespace infinity {

void
PhysicalCrossProduct::Init() {

}

void
PhysicalCrossProduct::Execute(SharedPtr<QueryContext>& query_context) {
    ExecutorAssert(left_->output() != nullptr, "No left input.");
    ExecutorAssert(right_->output() != nullptr, "No right input.");
    left_table_ = left_->output();
    right_table_ = right_->output();

    Vector<SharedPtr<ColumnDef>> columns_def;
    SizeT left_column_count = left_table_->ColumnCount();
    SizeT right_column_count = right_table_->ColumnCount();
    columns_def.reserve(left_column_count + right_column_count);

    {
        i64 column_idx{0};
        const Vector<SharedPtr<ColumnDef>> &left_column_defs = left_table_->definition_ptr_->columns();
        for (const SharedPtr<ColumnDef> &input_col_def: left_column_defs) {
            SharedPtr<ColumnDef> output_col_def = MakeShared<ColumnDef>(column_idx,
                                                                        input_col_def->type(),
                                                                        input_col_def->name(),
                                                                        input_col_def->constraints_);

            columns_def.emplace_back(output_col_def);
            ++column_idx;
        }

        const Vector<SharedPtr<ColumnDef>> &right_column_defs = right_table_->definition_ptr_->columns();
        for (const SharedPtr<ColumnDef> &input_col_def: right_column_defs) {
            SharedPtr<ColumnDef> output_col_def = MakeShared<ColumnDef>(column_idx,
                                                                        input_col_def->type(),
                                                                        input_col_def->name(),
                                                                        input_col_def->constraints_);
            columns_def.emplace_back(output_col_def);
            ++column_idx;
        }
    }

    SharedPtr<TableDef> cross_product_table_def = TableDef::Make("cross_product", columns_def);
    SharedPtr<Table> cross_product_table = Table::Make(cross_product_table_def, TableType::kCrossProduct);

    // Loop left table and scan right table
    SizeT left_block_count = left_table_->DataBlockCount();
    SizeT right_block_count = right_table_->DataBlockCount();
    for(const SharedPtr<DataBlock>& left_block: left_table_->data_blocks_) {
        for(const SharedPtr<DataBlock>& right_block: right_table_->data_blocks_) {
            // each row of left block will generate the constant column vectors and corresponding right column vectors
            SizeT output_row_count = right_block->row_count();

            SizeT left_row_count = left_block->row_count();
            for(SizeT row_idx = 0; row_idx < left_row_count; ++ row_idx) {
                // left block column vectors
                Vector<SharedPtr<ColumnVector>> output_columns;
                output_columns.reserve(left_column_count + right_column_count);

                // Prepare the left columns
                for(SizeT column_idx = 0; column_idx < left_column_count; ++ column_idx) {
                    const SharedPtr<ColumnVector>& left_column_vector = left_block->column_vectors[column_idx];

                    // Generate output column vector
                    SharedPtr<ColumnVector> column_vector = ColumnVector::Make(left_column_vector->data_type());

                    // From left block, it will be constant column vector, with right block capacity(DEFAULT_VECTOR_SIZE)
                    column_vector->Initialize(ColumnVectorType::kConstant, right_block->capacity());

                    // Fill the value.
                    column_vector->CopyRow(*left_column_vector, 0, row_idx);

                    output_columns.emplace_back(column_vector);
                }

                // Prepare the right columns
                for(SizeT column_idx = 0; column_idx < right_column_count; ++ column_idx) {
                    const SharedPtr<ColumnVector>& right_column_vector = right_block->column_vectors[column_idx];
                    output_columns.emplace_back(right_column_vector);
                }

                SharedPtr<DataBlock> output_block = DataBlock::Make();
                output_block->Init(output_columns);
                output_block->Finalize();
                cross_product_table->Append(output_block);
            }
        }
    }

    output_ = cross_product_table;
}

SharedPtr<Vector<String>>
PhysicalCrossProduct::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SharedPtr<Vector<String>> left_output_names = left_->GetOutputNames();
    SharedPtr<Vector<String>> right_output_names = right_->GetOutputNames();

    result->reserve(left_output_names->size() + right_output_names->size());
    for(auto& name_str: *left_output_names) {
        result->emplace_back(name_str);
    }

    for(auto& name_str: *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

}


