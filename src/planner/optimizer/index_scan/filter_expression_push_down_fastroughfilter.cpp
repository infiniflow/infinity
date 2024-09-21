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

#include <string>
#include <vector>
module filter_expression_push_down;

import stl;
import index_filter_expression_info_tree;
import value;
import expression_type;
import logical_type;
import internal_types;
import function_expression;
import filter_expression_push_down_helper;
import infinity_exception;
import third_party;

namespace infinity {

class FastRoughFilterEvaluatorTrue final : public FastRoughFilterEvaluator {
public:
    FastRoughFilterEvaluatorTrue() : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kAlwaysTrue) {}
    ~FastRoughFilterEvaluatorTrue() override = default;
    bool EvaluateInner(TxnTimeStamp, const FastRoughFilter &) const override { return true; }
};

class FastRoughFilterEvaluatorFalse final : public FastRoughFilterEvaluator {
public:
    FastRoughFilterEvaluatorFalse() : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kAlwaysFalse) {}
    ~FastRoughFilterEvaluatorFalse() override = default;
    bool EvaluateInner(TxnTimeStamp, const FastRoughFilter &) const override { return false; }
};

class FastRoughFilterEvaluatorCombineAnd final : public FastRoughFilterEvaluator {
    UniquePtr<FastRoughFilterEvaluator> left_;
    UniquePtr<FastRoughFilterEvaluator> right_;

public:
    FastRoughFilterEvaluatorCombineAnd(UniquePtr<FastRoughFilterEvaluator> left, UniquePtr<FastRoughFilterEvaluator> right)
        : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kCombineAnd), left_(std::move(left)), right_(std::move(right)) {}
    ~FastRoughFilterEvaluatorCombineAnd() override = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const override {
        return left_->EvaluateInner(query_ts, filter) and right_->EvaluateInner(query_ts, filter);
    }
};

class FastRoughFilterEvaluatorCombineOr final : public FastRoughFilterEvaluator {
    UniquePtr<FastRoughFilterEvaluator> left_;
    UniquePtr<FastRoughFilterEvaluator> right_;

public:
    FastRoughFilterEvaluatorCombineOr(UniquePtr<FastRoughFilterEvaluator> left, UniquePtr<FastRoughFilterEvaluator> right)
        : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kCombineOr), left_(std::move(left)), right_(std::move(right)) {}
    ~FastRoughFilterEvaluatorCombineOr() override = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const override {
        return left_->EvaluateInner(query_ts, filter) or right_->EvaluateInner(query_ts, filter);
    }
};

// fast "equal" filter
class FastRoughFilterEvaluatorProbabilisticDataFilter final : public FastRoughFilterEvaluator {
    ColumnID column_id_;
    Value value_;

public:
    FastRoughFilterEvaluatorProbabilisticDataFilter(const ColumnID column_id, Value value)
        : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kProbabilisticDataFilter), column_id_(column_id), value_(std::move(value)) {}
    ~FastRoughFilterEvaluatorProbabilisticDataFilter() override = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const override {
        return filter.MayContain(query_ts, column_id_, value_);
    }
};

// fast "range" filter
class FastRoughFilterEvaluatorMinMaxFilter final : public FastRoughFilterEvaluator {
    ColumnID column_id_;
    Value value_;
    FilterCompareType compare_type_;

public:
    FastRoughFilterEvaluatorMinMaxFilter(const ColumnID column_id, Value value, const FilterCompareType compare_type)
        : FastRoughFilterEvaluator(FastRoughFilterEvaluatorTag::kMinMaxFilter), column_id_(column_id), value_(std::move(value)),
          compare_type_(compare_type) {}
    ~FastRoughFilterEvaluatorMinMaxFilter() override = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const override {
        return filter.MayInRange(column_id_, value_, compare_type_);
    }
};

struct ExpressionFastRoughFilterInfo {
    enum class Enum {
        // mysterious expr
        kUnknownExpr,

        // constant value
        kValueExpr,
        // column
        kColumnExprOrAfterCast,

        // compare expr
        kColumnValueCompareExpr,
        kValueColumnCompareExpr,

        // logical expr ("not" is treated as unknown)
        kAndExpr,
        kOrExpr,
    };

    ExpressionInfoTree<ExpressionFastRoughFilterInfo> BuildTree(const SharedPtr<BaseExpression> &expression, const u32 depth = 0) const {
        ExpressionInfoTree<ExpressionFastRoughFilterInfo> tree;
        tree.src_ptr = &expression;
        switch (expression->type()) {
            case ExpressionType::kValue: {
                tree.info = Enum::kValueExpr;
                break;
            }
            case ExpressionType::kColumn: {
                if (expression->Type().SupportMinMaxFilter()) {
                    tree.info = Enum::kColumnExprOrAfterCast;
                }
                break;
            }
            case ExpressionType::kCast: {
                CommonCheckCast(this, tree, expression, depth);
                break;
            }
            case ExpressionType::kFunction: {
                tree.children.reserve(expression->arguments().size());
                for (const auto &child_expression : expression->arguments()) {
                    tree.children.push_back(BuildTree(child_expression, depth + 1));
                }
                // 1. all value
                // 2. and, or
                // 3. col, val, >, <, >=, <=, =
                { // 1.
                    bool all_value = true;
                    for (const auto &child : tree.children) {
                        if (child.info != Enum::kValueExpr) {
                            all_value = false;
                            break;
                        }
                    }
                    if (all_value) {
                        tree.info = Enum::kValueExpr;
                        break;
                    }
                }
                { // 2. 3. now not all value
                    auto *function_expression = static_cast<const FunctionExpression *>(expression.get());
                    if (const auto &f_name = function_expression->ScalarFunctionName();
                        std::find(LogicalFunctionNames.begin(), LogicalFunctionNames.end(), f_name) != LogicalFunctionNames.end()) {
                        // 1. all column / unknown children: unknown
                        // 2. other: and / or
                        bool all_column_or_unknown = true;
                        for (const auto &child : tree.children) {
                            if (child.info != Enum::kColumnExprOrAfterCast && child.info != Enum::kUnknownExpr) {
                                all_column_or_unknown = false;
                                break;
                            }
                        }
                        if (!all_column_or_unknown) {
                            tree.info = f_name == "AND" ? Enum::kAndExpr : Enum::kOrExpr;
                        }
                    } else if (std::find(CompareFunctionNames.begin(), CompareFunctionNames.end(), f_name) != CompareFunctionNames.end()) {
                        if (tree.children[0].info == Enum::kColumnExprOrAfterCast and tree.children[1].info == Enum::kValueExpr) {
                            tree.info = Enum::kColumnValueCompareExpr;
                        } else if (tree.children[0].info == Enum::kValueExpr and tree.children[1].info == Enum::kColumnExprOrAfterCast) {
                            tree.info = Enum::kValueColumnCompareExpr;
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
        return tree;
    }
};

class FastRoughFilterExpressionPushDownMethod {
    using TreeT = ExpressionInfoTree<ExpressionFastRoughFilterInfo>;
    using Enum = ExpressionFastRoughFilterInfo::Enum;

    static inline UniquePtr<FastRoughFilterEvaluator> ReturnAlwaysTrue() { return MakeUnique<FastRoughFilterEvaluatorTrue>(); }

    static inline UniquePtr<FastRoughFilterEvaluator> ReturnAlwaysFalse() { return MakeUnique<FastRoughFilterEvaluatorFalse>(); }

    static inline UniquePtr<FastRoughFilterEvaluator> ReturnValue(const SharedPtr<BaseExpression> &expression) {
        auto value = FilterExpressionPushDownHelper::CalcValueResult(expression);
        if (value.type().type() == LogicalType::kBoolean && !value.GetValue<BooleanT>()) {
            return ReturnAlwaysFalse();
        }
        return ReturnAlwaysTrue();
    }

    static inline UniquePtr<FastRoughFilterEvaluator> GetFastRoughFilterFromtreeNode(const TreeT &tree_node) {
        switch (tree_node.info) {
            case Enum::kUnknownExpr:
            case Enum::kColumnExprOrAfterCast: {
                return ReturnAlwaysTrue();
            }
            case Enum::kValueExpr: {
                return ReturnValue(*tree_node.src_ptr);
            }
            case Enum::kColumnValueCompareExpr:
            case Enum::kValueColumnCompareExpr: {
                auto *function_expression = static_cast<FunctionExpression *>(tree_node.src_ptr->get());
                auto const &f_name = function_expression->ScalarFunctionName();
                if (f_name == "=") {
                    auto SolveForExpr1 = [](SharedPtr<BaseExpression> &col_expr,
                                            SharedPtr<BaseExpression> &val_expr) -> UniquePtr<FastRoughFilterEvaluator> {
                        auto val_right = FilterExpressionPushDownHelper::CalcValueResult(val_expr);
                        auto [column_id, value, compare_type] =
                            FilterExpressionPushDownHelper::UnwindCast(col_expr, std::move(val_right), FilterCompareType::kEqual);
                        switch (compare_type) {
                            case FilterCompareType::kEqual: {
                                switch (value.type().type()) {
                                    case LogicalType::kBoolean:
                                    case LogicalType::kDecimal: {
                                        return MakeUnique<FastRoughFilterEvaluatorProbabilisticDataFilter>(column_id, std::move(value));
                                    }
                                    case LogicalType::kFloat:
                                    case LogicalType::kDouble: {
                                        auto minmax_filter_le =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kLessEqual);
                                        auto minmax_filter_ge =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kGreaterEqual);
                                        return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(minmax_filter_le),
                                                                                              std::move(minmax_filter_ge));
                                    }
                                    default: {
                                        auto minmax_filter_le =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kLessEqual);
                                        auto minmax_filter_ge =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kGreaterEqual);
                                        auto minmax_filter =
                                            MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(minmax_filter_le), std::move(minmax_filter_ge));
                                        auto bloom_filter = MakeUnique<FastRoughFilterEvaluatorProbabilisticDataFilter>(column_id, std::move(value));
                                        return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(bloom_filter), std::move(minmax_filter));
                                    }
                                }
                            }
                            case FilterCompareType::kInvalid: // special cast expression, e.g., cast varchar column to int and compare with int
                            case FilterCompareType::kAlwaysTrue: {
                                return ReturnAlwaysTrue();
                            }
                            case FilterCompareType::kAlwaysFalse: {
                                return ReturnAlwaysFalse();
                            }
                            default: {
                                // error
                                UnrecoverableError("Wrong compare type!");
                                return ReturnAlwaysTrue();
                            }
                        }
                    };
                    switch (tree_node.info) {
                        case Enum::kColumnValueCompareExpr: {
                            return SolveForExpr1(function_expression->arguments()[0], function_expression->arguments()[1]);
                        }
                        case Enum::kValueColumnCompareExpr: {
                            return SolveForExpr1(function_expression->arguments()[1], function_expression->arguments()[0]);
                        }
                        default: {
                            UnrecoverableError("Wrong expression type!");
                            return {};
                        }
                    }
                }
                constexpr std::array Case2FunctionNames{"<", ">", "<=", ">="};
                constexpr std::array Case2CompareTypes{FilterCompareType::kLess,
                                                       FilterCompareType::kGreater,
                                                       FilterCompareType::kLessEqual,
                                                       FilterCompareType::kGreaterEqual};
                constexpr std::array Case2ReverseCompareTypes{FilterCompareType::kGreater,
                                                              FilterCompareType::kLess,
                                                              FilterCompareType::kGreaterEqual,
                                                              FilterCompareType::kLessEqual};
                if (auto it = std::find(Case2FunctionNames.begin(), Case2FunctionNames.end(), f_name); it != Case2FunctionNames.end()) {
                    // maybe known expression 2
                    auto SolveForExpr2 = [](SharedPtr<BaseExpression> &col_expr,
                                            SharedPtr<BaseExpression> &val_expr,
                                            FilterCompareType initial_compare_type) -> UniquePtr<FastRoughFilterEvaluator> {
                        auto val_right = FilterExpressionPushDownHelper::CalcValueResult(val_expr);
                        auto [column_id, value, compare_type] =
                            FilterExpressionPushDownHelper::UnwindCast(col_expr, std::move(val_right), initial_compare_type);
                        switch (compare_type) {
                            case FilterCompareType::kLessEqual:
                            case FilterCompareType::kGreaterEqual: {
                                return MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, std::move(value), compare_type);
                            }
                            case FilterCompareType::kInvalid: // special cast expression, e.g., cast varchar column to int and compare with int
                            case FilterCompareType::kAlwaysTrue: {
                                return ReturnAlwaysTrue();
                            }
                            case FilterCompareType::kAlwaysFalse: {
                                return ReturnAlwaysFalse();
                            }
                            default: {
                                // error
                                UnrecoverableError("Wrong compare type!");
                                return ReturnAlwaysTrue();
                            }
                        }
                    };
                    switch (tree_node.info) {
                        case Enum::kColumnValueCompareExpr: {
                            return SolveForExpr2(function_expression->arguments()[0],
                                                 function_expression->arguments()[1],
                                                 Case2CompareTypes[std::distance(Case2FunctionNames.begin(), it)]);
                        }
                        case Enum::kValueColumnCompareExpr: {
                            return SolveForExpr2(function_expression->arguments()[1],
                                                 function_expression->arguments()[0],
                                                 Case2ReverseCompareTypes[std::distance(Case2FunctionNames.begin(), it)]);
                        }
                        default: {
                            UnrecoverableError("Wrong expression type!");
                            return {};
                        }
                    }
                }
                UnrecoverableError("Wrong function name!");
                return {};
            }
            case Enum::kAndExpr: {
                std::vector<UniquePtr<FastRoughFilterEvaluator>> children;
                for (const auto &child : tree_node.children) {
                    auto child_filter = GetFastRoughFilterFromtreeNode(child);
                    if (child_filter->Tag() == FastRoughFilterEvaluatorTag::kAlwaysFalse) {
                        return ReturnAlwaysFalse();
                    }
                    if (child_filter->Tag() != FastRoughFilterEvaluatorTag::kAlwaysTrue) {
                        children.push_back(std::move(child_filter));
                    }
                }
                if (children.empty()) {
                    return ReturnAlwaysTrue();
                }
                if (children.size() == 1) {
                    return std::move(children[0]);
                }
                if (children.size() == 2) {
                    return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(children[0]), std::move(children[1]));
                }
                UnrecoverableError("Wrong number of children in AND expression!");
                return {};
            }
            case Enum::kOrExpr: {
                std::vector<UniquePtr<FastRoughFilterEvaluator>> children;
                for (const auto &child : tree_node.children) {
                    auto child_filter = GetFastRoughFilterFromtreeNode(child);
                    if (child_filter->Tag() == FastRoughFilterEvaluatorTag::kAlwaysTrue) {
                        return ReturnAlwaysTrue();
                    }
                    if (child_filter->Tag() != FastRoughFilterEvaluatorTag::kAlwaysFalse) {
                        children.push_back(std::move(child_filter));
                    }
                }
                if (children.empty()) {
                    return ReturnAlwaysFalse();
                }
                if (children.size() == 1) {
                    return std::move(children[0]);
                }
                if (children.size() == 2) {
                    return MakeUnique<FastRoughFilterEvaluatorCombineOr>(std::move(children[0]), std::move(children[1]));
                }
                UnrecoverableError("Wrong number of children in OR expression!");
                return {};
            }
        }
    }

public:
    static inline UniquePtr<FastRoughFilterEvaluator> GetFastRoughFilter(SharedPtr<BaseExpression> &expression) {
        if (!expression) {
            return ReturnAlwaysTrue();
        }
        // keep old tree structure unchanged
        // build boolean evaluator tree
        // evaluate result: false: can skip block or segment, true: cannot skip block or segment
        // replace some known expression with FastRoughFilterEvaluator
        // replace others as "true"
        // known expression 1: "[cast] x equal value_expr" for ProbabilisticDataFilter, also need to build a val <= x <= val filter for MinMaxFilter
        // known expression 2: "[cast] x compare (>, <, >=, <=) value_expr" for MinMaxFilter
        // known expression 3 : "and" or "or" expression
        ExpressionFastRoughFilterInfo tree_info;
        const auto tree = tree_info.BuildTree(expression);
        return GetFastRoughFilterFromtreeNode(tree);
    }
};

UniquePtr<FastRoughFilterEvaluator> FilterExpressionPushDown::PushDownToFastRoughFilter(SharedPtr<BaseExpression> &expression) {
    return FastRoughFilterExpressionPushDownMethod::GetFastRoughFilter(expression);
}

} // namespace infinity
