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

export module infinity_core:physical_parallel_aggregate;

import :stl;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :load_meta;
import :infinity_exception;
import internal_types;
import data_type;
import :logger;

namespace infinity {

export class PhysicalParallelAggregate final : public PhysicalOperator {
public:
    explicit PhysicalParallelAggregate(SharedPtr<Vector<String>> output_names,
                                       SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                       u64 id,
                                       SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kParallelAggregate, nullptr, nullptr, id, load_metas), output_names_(std::move(output_names)),
          output_types_(std::move(output_types)) {}

    ~PhysicalParallelAggregate() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
