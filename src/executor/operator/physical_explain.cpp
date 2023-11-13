// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

import infinity_exception;

module physical_explain;

namespace infinity {

void PhysicalExplain::AlignParagraphs(Vector<SharedPtr<String>> &array1, Vector<SharedPtr<String>> &array2) {
    Vector<SizeT> paragraph_indices_1;
    Vector<SizeT> paragraph_indices_2;

    for (SizeT i = 0; i < array1.size(); ++i) {
        if (array1[i]->empty()) {
            paragraph_indices_1.push_back(i);
        }
    }
    for (SizeT i = 0; i < array2.size(); ++i) {
        if (array2[i]->empty()) {
            paragraph_indices_2.push_back(i);
        }
    }

    SizeT min_paragraphs = Min(paragraph_indices_1.size(), paragraph_indices_2.size());

    for (SizeT i = 0; i < min_paragraphs; ++i) {
        SizeT start_1 = (i == 0) ? 0 : paragraph_indices_1[i - 1] + 1;
        SizeT start_2 = (i == 0) ? 0 : paragraph_indices_2[i - 1] + 1;

        SizeT end1 = paragraph_indices_1[i];
        SizeT end2 = paragraph_indices_2[i];

        Vector<SharedPtr<String>> paragraphs1(array1.begin() + start_1, array1.begin() + end1);
        Vector<SharedPtr<String>> paragraphs2(array2.begin() + start_2, array2.begin() + end2);

        SizeT length_diff = paragraphs1.size() - paragraphs2.size();

        if (length_diff > 0) {
            array2.insert(array2.begin() + end2, length_diff, MakeShared<String>());
        } else if (length_diff < 0) {
            array1.insert(array1.begin() + end1, -length_diff, MakeShared<String>());
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
            Error<NotImplementException>("Not implement: Query analyze");
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

void PhysicalExplain::Execute(QueryContext *query_context, OperatorState *operator_state) {
    String title;

    auto column_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    auto task_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));

    auto output_data_block = DataBlock::Make();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            title = "Query Analyze";
            Error<NotImplementException>("Not implement: Query analyze");
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
        AlignParagraphs(*this->texts_, *this->task_texts_);

        for (SizeT idx = 0; idx < this->task_texts_->size(); ++idx) {
            task_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->task_texts_)[idx]));
        }
    }

    Vector<SharedPtr<ColumnVector>> column_vectors;
    column_vectors.reserve(2);

    column_vectors.push_back(column_vector_ptr);
    if (explain_type_ == ExplainType::kPipeline) {
        column_vectors.push_back(task_vector_ptr);
    }
    output_data_block->Init(column_vectors);

    ExplainOperatorState *explain_operator_state = static_cast<ExplainOperatorState *>(operator_state);
    explain_operator_state->data_block_ = output_data_block;
    operator_state->SetComplete();
}

} // namespace infinity
