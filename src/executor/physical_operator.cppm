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
import physical_operator_type;
import query_context;
import operator_state;
import parser;
import data_table;

export module physical_operator;

namespace infinity {

export class PhysicalOperator : public EnableSharedFromThis<PhysicalOperator> {

public:
    inline explicit PhysicalOperator(PhysicalOperatorType type, SharedPtr<PhysicalOperator> left, SharedPtr<PhysicalOperator> right, u64 id)
        : operator_id_(id), operator_type_(type), left_(Move(left)), right_(Move(right)) {}

    virtual ~PhysicalOperator() = default;

    virtual void Init() = 0;

    SharedPtr<PhysicalOperator> left() const { return left_; }

    SharedPtr<PhysicalOperator> right() const { return right_; }

    u64 node_id() const { return operator_id_; }

    /// for push based execution
    virtual void Execute(QueryContext *query_context, OperatorState *output_state) = 0;

    const SharedPtr<DataTable> &output() const { return output_; }

    inline PhysicalOperatorType operator_type() const { return operator_type_; }

    virtual SharedPtr<Vector<String>> GetOutputNames() const = 0;

    virtual SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const = 0;

    virtual String GetName() const;

protected:
    u64 operator_id_;
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<PhysicalOperator> left_{nullptr};
    SharedPtr<PhysicalOperator> right_{nullptr};

    SharedPtr<DataTable> output_{};

public:
    // Operator

    virtual bool ParallelOperator() const { return false; }

public:
    // Exchange
    virtual bool IsExchange() const { return false; }

    virtual bool ParallelExchange() const { return false; }

public:
    // Sink
    virtual bool IsSink() const { return false; }

    virtual bool ParallelSink() const { return false; }

    virtual bool SinkOrderMatters() const { return false; }
};

} // namespace infinity
