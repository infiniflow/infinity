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
export module filter_fulltext_expression;
import base_expression;
import data_type;
import stl;
import function_expr;
import filter_expression_push_down;
import roaring_bitmap;
import txn;
import block_index;

namespace infinity {

export class FilterFulltextExpression final : public BaseExpression {
public:
    FilterFulltextExpression(String fields, String matching_text, String options_text);

    static SharedPtr<FilterFulltextExpression> BuildFilterFulltextExpression(const FunctionExpr &expr);

    String ToString() const override;

    DataType Type() const override;

    String fields_;
    String matching_text_;
    String options_text_;

    // for expression_evaluator
    Txn *txn_ = nullptr;
    SharedPtr<BlockIndex> block_index_;
    UniquePtr<IndexFilterEvaluator> filter_fulltext_evaluator_;
    Map<SegmentID, Bitmask> segment_results_;
    std::shared_mutex rw_mutex_;
};

} // namespace infinity
