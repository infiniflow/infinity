//
// Created by jinhai on 23-3-13.
//

#include "physical_explain.h"

namespace infinity {

void
PhysicalExplain::Init() {
//    NotImplementError("Explain::Init")
}

void
PhysicalExplain::Execute(SharedPtr<QueryContext> &query_context) {
    String title;

    DataType data_type(LogicalType::kVarchar);
    auto column_vector_ptr = ColumnVector::Make(data_type);

    auto output_data_block = DataBlock::Make();

    switch(explain_type_) {
        case ExplainType::kAnalyze: {
            title = "Query Analyze";
            break;
        }
        case ExplainType::kAst: {
            title = "Abstract Syntax Tree";

            SizeT row_count = this->texts_->size();
            SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

            column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
            for(SizeT idx = 0; idx < row_count; ++ idx) {
                Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
                column_vector_ptr->AppendValue(str_v);
            }
            output_data_block->Init({column_vector_ptr});
            break;
        }
        case ExplainType::kUnOpt: {
            title = "Unoptimized Logical Plan";
            SizeT row_count = this->texts_->size();
            SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

            column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
            for(SizeT idx = 0; idx < row_count; ++ idx) {
                Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
                column_vector_ptr->AppendValue(str_v);
            }
            output_data_block->Init({column_vector_ptr});
            break;
        }
        case ExplainType::kOpt: {
            title = "Optimized Logical Plan";
            SizeT row_count = this->texts_->size();
            SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

            column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
            for(SizeT idx = 0; idx < row_count; ++ idx) {
                Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
                column_vector_ptr->AppendValue(str_v);
            }
            output_data_block->Init({column_vector_ptr});
            break;
        }
        case ExplainType::kPhysical: {
            title = "Physical Plan";
            SizeT row_count = this->texts_->size();
            SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

            column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
            for(SizeT idx = 0; idx < row_count; ++ idx) {
                Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
                column_vector_ptr->AppendValue(str_v);
            }
            output_data_block->Init({column_vector_ptr});
            break;
        }
        case ExplainType::kPipeline: {
            title = "Pipeline";
            break;
        }
    }

    Vector<SharedPtr<ColumnDef>> column_defs = {
            MakeShared<ColumnDef>(0, DataType(LogicalType::kVarchar), title, HashSet<ConstraintType>())
    };

    SharedPtr<TableDef> result_table_def_ptr
            = MakeShared<TableDef>("Tables", column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
    output_->Append(output_data_block);
}

}
