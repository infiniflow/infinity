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

export module infinity_core:physical_explain;

import :stl;
import :physical_operator;
import explain_statement;
import :physical_operator_type;

namespace infinity {
class QueryProfiler;
class PlanFragment;

export class PhysicalExplain final : public PhysicalOperator {
public:
    explicit PhysicalExplain(u64 id,
                             ExplainType type,
                             SharedPtr<Vector<SharedPtr<String>>> text_array,
                             UniquePtr<PhysicalOperator> left,
                             SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kExplain, std::move(left), nullptr, id, load_metas), explain_type_(type),
          texts_(std::move(text_array)) {}

    ~PhysicalExplain() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    void SetExplainText(SharedPtr<Vector<SharedPtr<String>>> text) { texts_ = std::move(text); }

    void SetExplainTaskText(SharedPtr<Vector<SharedPtr<String>>> text) { task_texts_ = std::move(text); }

    void SetPlanFragment(PlanFragment *plan_fragment_ptr);

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ExplainType explain_type() const { return explain_type_; }

    static void AlignParagraphs(Vector<SharedPtr<String>> &array1, Vector<SharedPtr<String>> &array2);

private:
    void ExplainAnalyze(Vector<SharedPtr<String>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler);
    void ExplainPipeline(Vector<SharedPtr<String>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler);

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    SharedPtr<Vector<SharedPtr<String>>> texts_{nullptr};
    SharedPtr<Vector<SharedPtr<String>>> task_texts_{nullptr};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    PlanFragment *plan_fragment_ptr_;
};

} // namespace infinity
