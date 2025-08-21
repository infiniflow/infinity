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

export module infinity_core:filter_fulltext_expression;

import :base_expression;

import :filter_expression_push_down;
import :roaring_bitmap;

import function_expr;
import data_type;

namespace infinity {

struct BlockIndex;

export class FilterFulltextExpression final : public BaseExpression {
public:
    FilterFulltextExpression(std::string fields, std::string matching_text, std::string options_text);

    ~FilterFulltextExpression() override;

    static std::shared_ptr<FilterFulltextExpression> BuildFilterFulltextExpression(const FunctionExpr &expr);

    std::string ToString() const override;

    DataType Type() const override;

    u64 Hash() const override;

    bool Eq(const BaseExpression &other_base) const override;

    std::string fields_;
    std::string matching_text_;
    std::string options_text_;

    // for expression_evaluator
    std::shared_ptr<BlockIndex> block_index_;
    std::unique_ptr<IndexFilterEvaluator> filter_fulltext_evaluator_;
    std::map<SegmentID, Bitmask> segment_results_;
    std::shared_mutex rw_mutex_;
};

} // namespace infinity
