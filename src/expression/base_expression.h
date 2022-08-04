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

    // Arithmetic operator
    kArithmetic,

    // Cast operator
    kCast,

    // Case operator
    kCase,

    // Conjunction operator
    kAnd,
    kOr,
    kNot,

    // Unbound column
    kColumn,

    // Bound column
    kBoundColumn,

    // Exists

    // Extract

    // Function,
    kFunction,

    // List,

    // Logical,
    kEqual,
    kLessThan,
    kGreaterThan,
    kLessThanEqual,
    kGreaterThanEqual,
    kBetween,

    // Unbound subquery
    kSubQuery,

    // Unary
    kUnaryMinus,
    kIsNull,

    // Value
    kValue,

    // IN
    kIn,

    // WINDOW Function
    kWindowRank,
    kWindowRowNumber,
};

class BaseExpression : public std::enable_shared_from_this<BaseExpression> {
public:
    explicit BaseExpression(ExpressionType type, std::vector<std::shared_ptr<BaseExpression>>  arguments)
        : type_(type), arguments_(std::move(arguments)) {};
    virtual ~BaseExpression() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;
    virtual LogicalType DataType() = 0;

protected:
    ExpressionType type_;
    std::vector<std::shared_ptr<BaseExpression>> arguments_;
};

}
