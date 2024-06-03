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

export module expression_type;

namespace infinity {

export enum class ExpressionType {
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
    kReference,

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
    kIsNotNull, // kNot + kIsNull ?

    // Value
    kValue,
    kDefault,   // ??
    kParameter, // ??

    // IN
    kIn,
    kNotIn, // kNot + kIn

    // WINDOW Function
    kWindowRank,
    kWindowRowNumber,

    // Search
    kSearch,
    kMatch,
    kFusion,
    kMatchTensor,
    kMatchSparse,

    // Misc.
    kKnn,
    kDistinctFrom,    // ??
    kNotDistinctFrom, // ??
    kPlaceholder,
    kPredicate, // ?
    kRaw,
};
}