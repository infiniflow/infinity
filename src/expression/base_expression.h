//
// Created by JinHai on 2022/8/4.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"

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
    explicit SourcePosition(i64 bind_context_id, ExprSourceType source_type)
        : bind_context_id_(bind_context_id), source_type_(source_type) {}

    i64 bind_context_id_{-1};
    ExprSourceType source_type_{ExprSourceType::kInvalid};
    String binding_name_;
};

class BaseExpression : public std::enable_shared_from_this<BaseExpression> {
public:
    explicit
    BaseExpression(ExpressionType type, Vector<SharedPtr<BaseExpression>> arguments)
        : type_(type), arguments_(std::move(arguments)) {};

    virtual
    ~BaseExpression() = default;

    [[nodiscard]] virtual String
    ToString() const = 0;

    virtual DataType
    Type() const = 0;

    [[nodiscard]] inline ExpressionType
    type() const {
        return type_;
    }

    inline Vector<SharedPtr<BaseExpression>>&
    arguments() {
        return arguments_;
    }

    SourcePosition source_position_;
    String alias_;

protected:
    ExpressionType type_;
    Vector<SharedPtr<BaseExpression>> arguments_;
};

}
