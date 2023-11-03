//
// Created by jinhai on 23-3-13.
//

module;

import stl;
import txn;
import query_context;
import table_def;
import data_table;
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

void PhysicalExplain::alignParagraphs(Vector<SharedPtr<String>>& array1, Vector<SharedPtr<String>>& array2) {
    Vector<SizeT> paragraphIndices1;
    Vector<SizeT> paragraphIndices2;

    for (SizeT i = 0; i < array1.size(); ++i) {
        if (array1[i]->empty()) {
            paragraphIndices1.push_back(i);
        }
    }
    for (SizeT i = 0; i < array2.size(); ++i) {
        if (array2[i]->empty()) {
            paragraphIndices2.push_back(i);
        }
    }

    SizeT maxParagraphs = Max(paragraphIndices1.size(), paragraphIndices2.size());

    for (SizeT i = 0; i < maxParagraphs; ++i) {
        if (i < paragraphIndices1.size() && i < paragraphIndices2.size()) {
            SizeT start1 = (i == 0) ? 0 : paragraphIndices1[i - 1] + 1;
            SizeT start2 = (i == 0) ? 0 : paragraphIndices2[i - 1] + 1;

            SizeT end1 = paragraphIndices1[i];
            SizeT end2 = paragraphIndices2[i];

            Vector<SharedPtr<String>> paragraphs1(array1.begin() + start1, array1.begin() + end1);
            Vector<SharedPtr<String>> paragraphs2(array2.begin() + start2, array2.begin() + end2);

            SizeT lengthDiff = paragraphs1.size() - paragraphs2.size();

            if (lengthDiff > 0) {
                array2.insert(array2.begin() + end2, lengthDiff, MakeShared<String>());
            } else if (lengthDiff < 0) {
                array1.insert(array1.begin() + end1, -lengthDiff, MakeShared<String>());
            }
        }
    }
}

void PhysicalExplain::Init() {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

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
        case ExplainType::kFragment: {
            output_names_->emplace_back("Fragment");
            break;
        }
        case ExplainType::kPipeline: {
            output_names_->emplace_back("Pipeline");
            output_names_->emplace_back("Task");
            break;
        }
    }
    output_types_->emplace_back(varchar_type);

    if (explain_type_ == ExplainType::kPipeline) {
        output_types_->emplace_back(varchar_type);
    }
}

void PhysicalExplain::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    String title;

    auto column_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    auto task_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));

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
        case ExplainType::kFragment: {
            title = "Fragment";
            break;
        }
        case ExplainType::kPipeline: {
            title = "Pipeline";
            break;
        }
    }

    SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

    column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
    task_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);

    for (SizeT idx = 0; idx < this->texts_->size(); ++idx) {
        column_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->texts_)[idx]));
    }
    if (explain_type_ == ExplainType::kPipeline) {
        for (SizeT idx = 0; idx < this->task_texts->size(); ++idx) {
            task_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->task_texts)[idx]));
        }

        alignParagraphs(*this->texts_, *this->task_texts);
    }

    Vector<SharedPtr<ColumnVector>> column_vectors;
    column_vectors.reserve(2);

    column_vectors.push_back(column_vector_ptr);
    if (explain_type_ == ExplainType::kPipeline) {
        column_vectors.push_back(task_vector_ptr);
    }
    output_data_block->Init(column_vectors);

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
        case ExplainType::kFragment: {
            title = "Fragment";
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
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    output_->Append(output_data_block);
}

} // namespace infinity
