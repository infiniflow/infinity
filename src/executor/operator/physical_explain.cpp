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

#include <vector>

module physical_explain;

import stl;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import column_vector;
import data_block;
import default_values;
import value;
import status;
import infinity_exception;
import logical_type;
import logger;
import plan_fragment;
import fragment_task;
import third_party;

namespace infinity {

void PhysicalExplain::AlignParagraphs(Vector<SharedPtr<String>> &array1, Vector<SharedPtr<String>> &array2) {
    SizeT size1 = array1.size();
    SizeT size2 = array2.size();

    if (size1 < size2) {
        array1.resize(size2, MakeShared<String>(""));
    } else if (size2 < size1) {
        array2.resize(size1, MakeShared<String>(""));
    }
}

void PhysicalExplain::Init(QueryContext *query_context) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            output_names_->emplace_back("Pipeline");
            output_names_->emplace_back("Task cost");
            break;
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
        case ExplainType::kInvalid: {
            String error_message = "Invalid explain type";
            UnrecoverableError(error_message);
        }
    }
    output_types_->emplace_back(varchar_type);

    if (explain_type_ == ExplainType::kPipeline or explain_type_ == ExplainType::kAnalyze) {
        output_types_->emplace_back(varchar_type);
    }
}

bool PhysicalExplain::Execute(QueryContext *query_context, OperatorState *operator_state) {
    String title;

    auto column_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    auto task_vector_ptr = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));

    auto output_data_block = DataBlock::MakeUniquePtr();

    switch (explain_type_) {
        case ExplainType::kAnalyze: {
            title = "Analyze";
            break;
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
        case ExplainType::kInvalid: {
            String error_message = "Invalid explain type";
            UnrecoverableError(error_message);
        }
    }

    SizeT capacity = DEFAULT_VECTOR_SIZE; // DEFAULT VECTOR SIZE is too large for it.

    column_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);
    task_vector_ptr->Initialize(ColumnVectorType::kFlat, capacity);

    if (explain_type_ == ExplainType::kPipeline) {
        Vector<SharedPtr<String>> task_texts;
        ExplainPipeline(task_texts, plan_fragment_ptr_, query_context->query_profiler());

        AlignParagraphs(*this->texts_, task_texts);
        for (SizeT idx = 0; idx < this->texts_->size(); ++idx) {
            column_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->texts_)[idx]));
        }
        for (SizeT idx = 0; idx < task_texts.size(); ++idx) {
            task_vector_ptr->AppendValue(Value::MakeVarchar(*task_texts[idx]));
        }
    } else if (explain_type_ == ExplainType::kAnalyze) {
        Vector<SharedPtr<String>> task_texts;
        ExplainAnalyze(task_texts, plan_fragment_ptr_, query_context->query_profiler());

        AlignParagraphs(*this->texts_, task_texts);
        for (SizeT idx = 0; idx < this->texts_->size(); ++idx) {
            column_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->texts_)[idx]));
        }
        for (SizeT idx = 0; idx < task_texts.size(); ++idx) {
            task_vector_ptr->AppendValue(Value::MakeVarchar(*task_texts[idx]));
        }

    } else {
        for (SizeT idx = 0; idx < this->texts_->size(); ++idx) {
            column_vector_ptr->AppendValue(Value::MakeVarchar(*(*this->texts_)[idx]));
        }
    }

    Vector<SharedPtr<ColumnVector>> column_vectors;
    column_vectors.reserve(2);

    column_vectors.push_back(column_vector_ptr);
    if (explain_type_ == ExplainType::kPipeline or explain_type_ == ExplainType::kAnalyze) {
        column_vectors.push_back(task_vector_ptr);
    }
    output_data_block->Init(column_vectors);

    ExplainOperatorState *explain_operator_state = static_cast<ExplainOperatorState *>(operator_state);
    explain_operator_state->data_block_array_.emplace_back(std::move(output_data_block));
    operator_state->SetComplete();
    return true;
}

void PhysicalExplain::SetPlanFragment(PlanFragment *plan_fragment_ptr) { plan_fragment_ptr_ = plan_fragment_ptr; }

void PhysicalExplain::ExplainAnalyze(Vector<SharedPtr<String>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler) {
    Vector<UniquePtr<FragmentTask>> &tasks = plan_fragment_ptr->GetContext()->Tasks();
    u64 fragment_id = plan_fragment_ptr->FragmentID();
    {
        String fragment_header = fmt::format("Fragment #{} * {} Tasks", fragment_id, tasks.size());
        result.emplace_back(MakeShared<String>(fragment_header));
    }
    for (const auto &task : tasks) {
        i64 task_id = task->TaskID();

        Vector<TaskProfiler> &task_profiles = query_profiler->GetTaskProfile(fragment_id, task_id);
        for (const auto &task_profile : task_profiles) {
            i64 times = 0;
            result.emplace_back(MakeShared<String>(fmt::format("-> Task {}, Seq: {}", task_id, times)));
            for (const auto &operator_info : task_profile.timings_) {
                String operator_info_str = fmt::format("  -> {} : ElapsedTime: {}, Output: {}",
                                                       operator_info.name_,
                                                       BaseProfiler::ElapsedToString(static_cast<infinity::NanoSeconds>(operator_info.elapsed_)),
                                                       operator_info.output_rows_);
                result.emplace_back(MakeShared<String>(operator_info_str));
            }
            ++times;
        }
    }
    // NOTE: Insert blank elements after each Fragment for alignment
    result.emplace_back(MakeShared<String>());

    if (plan_fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : plan_fragment_ptr->Children()) {
            ExplainAnalyze(result, child_fragment.get(), query_profiler);
        }
    }
}

void PhysicalExplain::ExplainPipeline(Vector<SharedPtr<String>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler) {
    Vector<UniquePtr<FragmentTask>> &tasks = plan_fragment_ptr->GetContext()->Tasks();
    u64 fragment_id = plan_fragment_ptr->FragmentID();
    {
        String fragment_header = fmt::format("Fragment #{} * {} Tasks", fragment_id, tasks.size());
        result.emplace_back(MakeShared<String>(fragment_header));
    }
    // NOTE: Insert blank elements after each Fragment for alignment
    result.emplace_back(MakeShared<String>());

    if (plan_fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : plan_fragment_ptr->Children()) {
            ExplainPipeline(result, child_fragment.get(), query_profiler);
        }
    }
}

} // namespace infinity
