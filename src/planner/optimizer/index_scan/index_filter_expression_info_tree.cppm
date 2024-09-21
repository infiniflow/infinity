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

export module index_filter_expression_info_tree;

import stl;
import logical_type;
import base_expression;
import infinity_exception;
import third_party;

namespace infinity {

export template <typename T>
struct ExpressionInfoTree {
    const SharedPtr<BaseExpression> *src_ptr = nullptr;
    Vector<ExpressionInfoTree> children{};
    typename T::Enum info = T::Enum::kUnknownExpr;
};

export template <typename T>
void CommonCheckCast(const T *src_info, ExpressionInfoTree<T> &tree, const SharedPtr<BaseExpression> &cast_expression, const u32 depth) {
    // check cast
    if (cast_expression->arguments().size() != 1) {
        UnrecoverableError(fmt::format("Expression depth: {}. Unsupported expression type: the cast expression {} has more than 1 argument.",
                                       depth,
                                       cast_expression->Name()));
    }
    // fill tree
    tree.children.push_back(src_info->BuildTree(cast_expression->arguments()[0], depth + 1));
    // check unwind compatibility
    // support:
    // case 1. cast "smaller ints" to BigIntT
    // case 2. cast "ints" or "floats" to DoubleT
    bool valid_cast = false;
    switch (cast_expression->Type().type()) {
        case LogicalType::kBigInt: {
            switch (cast_expression->arguments()[0]->Type().type()) {
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger: {
                    valid_cast = true;
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        case LogicalType::kDouble: {
            switch (cast_expression->arguments()[0]->Type().type()) {
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger:
                case LogicalType::kBigInt:
                case LogicalType::kFloat: {
                    valid_cast = true;
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    // TODO: allow more valid cast
    if (valid_cast) {
        tree.info = tree.children[0].info;
    } else {
        tree.info = T::Enum::kUnknownExpr;
    }
}

export constexpr std::array CompareFunctionNames{"<", ">", "<=", ">=", "="};
export constexpr std::array LogicalFunctionNames{"AND", "OR"};

} // namespace infinity