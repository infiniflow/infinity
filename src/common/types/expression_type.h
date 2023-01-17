//
// Created by jinhai on 23-1-22.
//

#pragma once

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
    kRaw,
};

}