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

export module infinity_core:filter_expression_push_down;

import :base_expression;
import :base_table_ref;
import :index_base;
import :fast_rough_filter;
import :roaring_bitmap;

namespace infinity {

class NewTxn;
class QueryContext;
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
    [[nodiscard]] virtual Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count) const = 0;

protected:
    Type type_;
    explicit IndexFilterEvaluator(const Type type) : type_(type) {}
};

export struct IndexScanFilterExpressionPushDownResult {
    std::shared_ptr<BaseExpression> index_filter_;
    std::shared_ptr<BaseExpression> leftover_filter_;

    std::unique_ptr<IndexFilterEvaluator> index_filter_evaluator_;
};

export class FilterExpressionPushDown {
public:
    static IndexScanFilterExpressionPushDownResult
    PushDownToIndexScan(QueryContext *query_context, const BaseTableRef *base_table_ref_ptr, const std::shared_ptr<BaseExpression> &expression);

    static void BuildFilterFulltextExpression(QueryContext *query_context,
                                              const BaseTableRef *base_table_ref_ptr,
                                              const std::vector<std::shared_ptr<BaseExpression>> &expressions);

    static std::unique_ptr<FastRoughFilterEvaluator> PushDownToFastRoughFilter(std::shared_ptr<BaseExpression> &expression);
};

} // namespace infinity