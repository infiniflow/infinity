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
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import load_meta;
import infinity_exception;

export module physical_optimize;

namespace infinity {

export class PhysicalOptimize final : public PhysicalOperator {
public:
    explicit PhysicalOptimize(u64 id, String db_name, String object_name, SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kOptimize, nullptr, nullptr, id, load_metas), db_name_(Move(db_name)),
          object_name_(Move(object_name)) {}

    ~PhysicalOptimize() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    inline OptimizeType optimize_type() const { return optimize_type_; }

private:
    void OptimizeIndex(QueryContext *query_context, OperatorState *operator_state);

private:
    OptimizeType optimize_type_{OptimizeType::kIRS};
    String db_name_{};
    String object_name_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
