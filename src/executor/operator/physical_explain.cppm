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

export module infinity_core:physical_explain;

import :physical_operator;
import :physical_operator_type;

import explain_statement;

namespace infinity {
class QueryProfiler;
class PlanFragment;

export class PhysicalExplain final : public PhysicalOperator {
public:
    explicit PhysicalExplain(u64 id,
                             ExplainType type,
                             std::shared_ptr<std::vector<std::shared_ptr<std::string>>> text_array,
                             std::unique_ptr<PhysicalOperator> left,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kExplain, std::move(left), nullptr, id, load_metas), explain_type_(type),
          texts_(std::move(text_array)) {}

    ~PhysicalExplain() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    void SetExplainText(std::shared_ptr<std::vector<std::shared_ptr<std::string>>> text) { texts_ = std::move(text); }

    void SetExplainTaskText(std::shared_ptr<std::vector<std::shared_ptr<std::string>>> text) { task_texts_ = std::move(text); }

    void SetPlanFragment(PlanFragment *plan_fragment_ptr);

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ExplainType explain_type() const { return explain_type_; }

    static void AlignParagraphs(std::vector<std::shared_ptr<std::string>> &array1, std::vector<std::shared_ptr<std::string>> &array2);

private:
    void ExplainAnalyze(std::vector<std::shared_ptr<std::string>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler);
    void ExplainPipeline(std::vector<std::shared_ptr<std::string>> &result, PlanFragment *plan_fragment_ptr, QueryProfiler *query_profiler);

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    std::shared_ptr<std::vector<std::shared_ptr<std::string>>> texts_{nullptr};
    std::shared_ptr<std::vector<std::shared_ptr<std::string>>> task_texts_{nullptr};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

    PlanFragment *plan_fragment_ptr_;
};

} // namespace infinity
