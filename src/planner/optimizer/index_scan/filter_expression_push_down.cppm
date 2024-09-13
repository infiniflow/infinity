//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module filter_expression_push_down;

import stl;
import query_context;
import base_expression;
import base_table_ref;
import index_base;
import fast_rough_filter;
import roaring_bitmap;
import txn;

namespace infinity {

// TODO: equivalent expression rewrite optimization

// TODO: now do not support "not" expression in index scan

// 1. secondary index
// 2. filter_fulltext
// 3. AND, OR
export struct IndexFilterEvaluator {
    enum class Type {
        kAllTrue,
        kAllFalse,
        kSecondaryIndex,
        kFulltextIndex,
        kAnd,
        kOr,
        kInvalid,
    };

    Type type() const { return type_; }
    virtual ~IndexFilterEvaluator() = default;
    [[nodiscard]] virtual Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count, Txn *txn) const = 0;

protected:
    Type type_;
    explicit IndexFilterEvaluator(const Type type) : type_(type) {}
};

export struct IndexScanFilterExpressionPushDownResult {
    SharedPtr<BaseExpression> index_filter_;
    SharedPtr<BaseExpression> leftover_filter_;

    UniquePtr<IndexFilterEvaluator> index_filter_evaluator_;
};

export class FilterExpressionPushDown {
public:
    static IndexScanFilterExpressionPushDownResult
    PushDownToIndexScan(QueryContext *query_context, const BaseTableRef &base_table_ref, const SharedPtr<BaseExpression> &expression);

    static UniquePtr<FastRoughFilterEvaluator> PushDownToFastRoughFilter(SharedPtr<BaseExpression> &expression);
};

} // namespace infinity