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
import value_expression;
import data_table;
import load_meta;
import infinity_exception;

export module physical_limit;

namespace infinity {

class DataBlock;

export class LimitCounter {
public:
    // Returns the left index after offset
    virtual SizeT Offset(SizeT row_count) = 0;

    // Returns the right index after limit
    virtual SizeT Limit(SizeT row_count) = 0;

    virtual bool IsLimitOver() = 0;
};

export class AtomicCounter : public LimitCounter {
public:
    AtomicCounter(i64 offset, i64 limit) : offset_(offset), limit_(limit) {}

    SizeT Offset(SizeT row_count);

    SizeT Limit(SizeT row_count);

    bool IsLimitOver();

private:
    ai64 offset_{};
    ai64 limit_{};
};

export class UnSyncCounter : public LimitCounter {
public:
    UnSyncCounter(i64 offset, i64 limit) : offset_(offset), limit_(limit) {}

    SizeT Offset(SizeT row_count);

    SizeT Limit(SizeT row_count);

    bool IsLimitOver();

private:
    i64 offset_{};
    i64 limit_{};
};

export class PhysicalLimit : public PhysicalOperator {
public:
    explicit PhysicalLimit(u64 id,
                           UniquePtr<PhysicalOperator> left,
                           SharedPtr<BaseExpression> limit_expr,
                           SharedPtr<BaseExpression> offset_expr,
                           SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalLimit() override = default;

    void Init() override;

    static bool Execute(QueryContext *query_context,
                        const Vector<UniquePtr<DataBlock>> &input_blocks,
                        Vector<UniquePtr<DataBlock>> &output_blocks,
                        LimitCounter *counter);

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    SizeT TaskletCount() override {
        return left_->TaskletCount();
    }

    inline const SharedPtr<BaseExpression> &limit_expr() const { return limit_expr_; }

    inline const SharedPtr<BaseExpression> &offset_expr() const { return offset_expr_; }

private:
    SharedPtr<BaseExpression> limit_expr_{};
    SharedPtr<BaseExpression> offset_expr_{};

    UniquePtr<LimitCounter> counter_{};
};

} // namespace infinity
