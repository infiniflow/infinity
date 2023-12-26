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
import data_table;
import load_meta;
import infinity_exception;

export module physical_top;

namespace infinity {

export class PhysicalTop : public PhysicalOperator {
public:
    explicit PhysicalTop(u64 id, SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kTop, nullptr, nullptr, id, load_metas) {}

    ~PhysicalTop() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }
};

} // namespace infinity
