//
// Created by JinHai on 2022/8/4.
//

#pragma once

#include "storage/data_type.h"

#include <memory>
#include <utility>
#include <vector>

namespace infinity {

enum class ExpressionType {
    // Aggregate function
    kAggregate,
    kGroupingFunction, // ??

    // Arithmetic operator
    kArithmetic,

    // Cast operator
    kCast,

    // Case operator
    kCase,

    // Conjunction operator
    kAnd,
    kConjunction,
    kOr,
    kNot,

    // Column
    kColumn,

    // Correlated column expression
    kCorrelatedColumn,

    // Exists
    kExists,

    // Extract
    kExtract,

    // Interval
    kInterval,

    // Function,
    kFunction,

    // List,
    kList,

    // Logical,
    kLogical,
    kEqual,
    kNotEqual, // kNot + kEqual ?
    kLessThan,
    kGreaterThan,
    kLessThanEqual,
    kGreaterThanEqual,
    kBetween,
    kNotBetween, // kNot + kBetween ?

    // Unbound subquery
    kSubQuery,

    // Unary
    kUnaryMinus,
    kIsNull,
    kIsNotNull,  // kNot + kIsNull ?

    // Value
    kValue,
    kDefault, // ??
    kParameter, // ??

    // IN
    kIn,
    kNotIn, // kNot + kIn

    // WINDOW Function
    kWindowRank,
    kWindowRowNumber,

    // Misc.
    kDistinctFrom, // ??
    kNotDistinctFrom, // ??
    kPlaceholder,
    kPredicate, // ?
};

enum class ExprSourceType {
    kInvalid,
    kAggregate,
    kGroupBy,
    kProjection,
    kBinding,
};

struct SourcePosition {
    SourcePosition() = default;
    explicit SourcePosition(int64_t bind_context_id, ExprSourceType source_type)
        : bind_context_id_(bind_context_id), source_type_(source_type) {}

    int64_t bind_context_id_{-1};
    ExprSourceType source_type_{ExprSourceType::kInvalid};
    std::string binding_name_;
};

class BaseExpression : public std::enable_shared_from_this<BaseExpression> {
public:
    explicit BaseExpression(ExpressionType type, std::vector<std::shared_ptr<BaseExpression>> arguments)
        : type_(type), arguments_(std::move(arguments)) {};
    virtual ~BaseExpression() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;
    virtual LogicalType DataType() = 0;
    [[nodiscard]] ExpressionType type() const { return type_; }
    std::vector<std::shared_ptr<BaseExpression>>& arguments() { return arguments_; }

    SourcePosition source_position_;

protected:
    ExpressionType type_;
    std::vector<std::shared_ptr<BaseExpression>> arguments_;
};

}
