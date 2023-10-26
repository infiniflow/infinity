//
// Created by jinhai on 23-3-13.
//

module;

import stl;
import txn;
import query_context;
import table_def;
import table;
import parser;
import physical_operator_type;
import operator_state;
import column_vector;
import data_block;
import default_values;
import value;
import infinity_assert;
import infinity_exception;

module physical_explain;

namespace infinity {

void PhysicalExplain::Init() {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            output_names_->emplace_back("Query Analyze");
            Error<NotImplementException>("Not implement: Query analyze", __FILE_NAME__, __LINE__);
        }
        case ExplainType::kAst: {
            output_names_->emplace_back("Abstract Syntax Tree");
            break;
        }
        case ExplainType::kUnOpt: {
            output_names_->emplace_back("Unoptimized Logical Plan");
            break;
        }
        case ExplainType::kOpt: {
            output_names_->emplace_back("Optimized Logical Plan");
            break;
        }
        case ExplainType::kPhysical: {
            output_names_->emplace_back("Physical Plan");
            break;
        }
        case ExplainType::kPipeline: {
            output_names_->emplace_back("Pipeline");
            break;
        }
    }
    output_types_->emplace_back(varchar_type);
}

void PhysicalExplain::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    String title;

    auto column_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));

    auto output_data_block = DataBlock::Make();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            title = "Query Analyze";
            Error<NotImplementException>("Not implement: Query analyze", __FILE_NAME__, __LINE__);
        }
        case ExplainType::kAst: {
            title = "Abstract Syntax Tree";
            break;
        }
        case ExplainType::kUnOpt: {
            title = "Unoptimized Logical Plan";
            break;
        }
        case ExplainType::kOpt: {
            title = "Optimized Logical Plan";
            break;
        }
        case ExplainType::kPhysical: {
            title = "Physical Plan";
            break;
        }
        case ExplainType::kPipeline: {
            title = "Pipeline";
            break;
        }
    }

    // Fill the explain text
    SizeT row_count = this->texts_->size();
    SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

    column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
    for (SizeT idx = 0; idx < row_count; ++idx) {
        Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
        column_vector_ptr->AppendValue(str_v);
    }
    output_data_block->Init({column_vector_ptr});

    ExplainOutputState *explain_output_state = static_cast<ExplainOutputState *>(output_state);
    explain_output_state->data_block_ = output_data_block;
    output_state->SetComplete();
}

void PhysicalExplain::Execute(QueryContext *query_context) {
    String title;

    auto column_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));

    auto output_data_block = DataBlock::Make();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            title = "Query Analyze";
            Error<NotImplementException>("Not implement: Query analyze", __FILE_NAME__, __LINE__);
        }
        case ExplainType::kAst: {
            title = "Abstract Syntax Tree";
            break;
        }
        case ExplainType::kUnOpt: {
            title = "Unoptimized Logical Plan";
            break;
        }
        case ExplainType::kOpt: {
            title = "Optimized Logical Plan";
            break;
        }
        case ExplainType::kPhysical: {
            title = "Physical Plan";
            break;
        }
        case ExplainType::kPipeline: {
            title = "Pipeline";
            break;
        }
    }

    // Fill the explain text
    SizeT row_count = this->texts_->size();
    SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

    column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
    for (SizeT idx = 0; idx < row_count; ++idx) {
        Value str_v = Value::MakeVarchar(*(*this->texts_)[idx]);
        column_vector_ptr->AppendValue(str_v);
    }
    output_data_block->Init({column_vector_ptr});

    // Prepare the output columns
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kVarchar), title, HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
    output_->Append(output_data_block);
}

} // namespace infinity
