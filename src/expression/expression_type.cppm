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

export module infinity_core:expression_type;

import std;

namespace infinity {

export enum class ExpressionType {
    kInvalid = -1,

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

    // std::list,
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

    // Fulltext filter
    kFilterFullText,

    // Unnest
    kUnnest,
};

std::string ExpressionType2Str(ExpressionType expression_type) {
    switch (expression_type) {
        case ExpressionType::kInvalid: {
            return ("Invalid");
        }
        case ExpressionType::kAggregate: {
            return ("Aggregate");
        }
        case ExpressionType::kGroupingFunction: {
            return ("GroupingFunction");
        }
        case ExpressionType::kArithmetic: {
            return ("Arithmetic");
        }
        case ExpressionType::kCast: {
            return ("Cast");
        }
        case ExpressionType::kCase: {
            return ("Case");
        }
        case ExpressionType::kAnd: {
            return ("And");
        }
        case ExpressionType::kConjunction: {
            return ("Conjunction");
        }
        case ExpressionType::kOr: {
            return ("Or");
        }
        case ExpressionType::kNot: {
            return ("Not");
        }
        case ExpressionType::kColumn: {
            return ("Column");
        }
        case ExpressionType::kReference: {
            return ("Reference");
        }
        case ExpressionType::kCorrelatedColumn: {
            return ("CorrelatedColumn");
        }
        case ExpressionType::kExists: {
            return ("Exists");
        }
        case ExpressionType::kExtract: {
            return ("Extract");
        }
        case ExpressionType::kInterval: {
            return ("Interval");
        }
        case ExpressionType::kFunction: {
            return ("Function");
        }
        case ExpressionType::kList: {
            return ("List");
        }
        case ExpressionType::kLogical: {
            return ("Logical");
        }
        case ExpressionType::kEqual: {
            return ("Equal");
        }
        case ExpressionType::kNotEqual: {
            return ("NotEqual");
        }
        case ExpressionType::kLessThan: {
            return ("LessThan");
            break;
        }
        case ExpressionType::kGreaterThan: {
            return ("GreaterThan");
        }
        case ExpressionType::kLessThanEqual: {
            return ("LessThanEqual");
        }
        case ExpressionType::kGreaterThanEqual: {
            return ("GreaterThanEqual");
        }
        case ExpressionType::kBetween: {
            return ("Between");
        }
        case ExpressionType::kNotBetween: {
            return ("NotBetween");
        }
        case ExpressionType::kSubQuery: {
            return ("SubQuery");
        }
        case ExpressionType::kUnaryMinus: {
            return ("UnaryMinus");
        }
        case ExpressionType::kIsNull: {
            return ("IsNull");
        }
        case ExpressionType::kIsNotNull: {
            return ("IsNotNull");
        }
        case ExpressionType::kValue: {
            return ("Value");
        }
        case ExpressionType::kDefault: {
            return ("Default");
        }
        case ExpressionType::kParameter: {
            return ("Parameter");
        }
        case ExpressionType::kIn: {
            return ("In");
        }
        case ExpressionType::kNotIn: {
            return ("NotIn");
        }
        case ExpressionType::kWindowRank: {
            return ("WindowRank");
        }
        case ExpressionType::kWindowRowNumber: {
            return ("WindowRowNumber");
        }
        case ExpressionType::kSearch: {
            return ("Search");
        }
        case ExpressionType::kMatch: {
            return ("Match");
        }
        case ExpressionType::kFusion: {
            return ("Fusion");
        }
        case ExpressionType::kMatchTensor: {
            return ("MatchTensor");
        }
        case ExpressionType::kMatchSparse: {
            return ("MatchSparse");
        }
        case ExpressionType::kKnn: {
            return ("Knn");
        }
        case ExpressionType::kDistinctFrom: {
            return ("DistinctFrom");
        }
        case ExpressionType::kNotDistinctFrom: {
            return ("NotDistinctFrom");
        }
        case ExpressionType::kPlaceholder: {
            return ("Placeholder");
        }
        case ExpressionType::kPredicate: {
            return ("Predicate");
        }
        case ExpressionType::kRaw: {
            return ("Raw");
        }
        case ExpressionType::kFilterFullText: {
            return ("FilterFullText");
        }
        case ExpressionType::kUnnest: {
            return ("Unnest");
        }
    }

    return "Invalid";
}

ExpressionType Str2ExpressionType(const std::string &str) {
    static const std::unordered_map<std::string, ExpressionType> str_to_type = {{"Aggregate", ExpressionType::kAggregate},
                                                                                {"GroupingFunction", ExpressionType::kGroupingFunction},
                                                                                {"Arithmetic", ExpressionType::kArithmetic},
                                                                                {"Cast", ExpressionType::kCast},
                                                                                {"Case", ExpressionType::kCase},
                                                                                {"And", ExpressionType::kAnd},
                                                                                {"Conjunction", ExpressionType::kConjunction},
                                                                                {"Or", ExpressionType::kOr},
                                                                                {"Not", ExpressionType::kNot},
                                                                                {"Column", ExpressionType::kColumn},
                                                                                {"Reference", ExpressionType::kReference},
                                                                                {"CorrelatedColumn", ExpressionType::kCorrelatedColumn},
                                                                                {"Exists", ExpressionType::kExists},
                                                                                {"Extract", ExpressionType::kExtract},
                                                                                {"Interval", ExpressionType::kInterval},
                                                                                {"Function", ExpressionType::kFunction},
                                                                                {"List", ExpressionType::kList},
                                                                                {"Logical", ExpressionType::kLogical},
                                                                                {"Equal", ExpressionType::kEqual},
                                                                                {"NotEqual", ExpressionType::kNotEqual},
                                                                                {"LessThan", ExpressionType::kLessThan},
                                                                                {"GreaterThan", ExpressionType::kGreaterThan},
                                                                                {"LessThanEqual", ExpressionType::kLessThanEqual},
                                                                                {"GreaterThanEqual", ExpressionType::kGreaterThanEqual},
                                                                                {"Between", ExpressionType::kBetween},
                                                                                {"NotBetween", ExpressionType::kNotBetween},
                                                                                {"SubQuery", ExpressionType::kSubQuery},
                                                                                {"UnaryMinus", ExpressionType::kUnaryMinus},
                                                                                {"IsNull", ExpressionType::kIsNull},
                                                                                {"IsNotNull", ExpressionType::kIsNotNull},
                                                                                {"Value", ExpressionType::kValue},
                                                                                {"Default", ExpressionType::kDefault},
                                                                                {"Parameter", ExpressionType::kParameter},
                                                                                {"In", ExpressionType::kIn},
                                                                                {"NotIn", ExpressionType::kNotIn},
                                                                                {"WindowRank", ExpressionType::kWindowRank},
                                                                                {"WindowRowNumber", ExpressionType::kWindowRowNumber},
                                                                                {"Search", ExpressionType::kSearch},
                                                                                {"Match", ExpressionType::kMatch},
                                                                                {"Fusion", ExpressionType::kFusion},
                                                                                {"MatchTensor", ExpressionType::kMatchTensor},
                                                                                {"MatchSparse", ExpressionType::kMatchSparse},
                                                                                {"Knn", ExpressionType::kKnn},
                                                                                {"DistinctFrom", ExpressionType::kDistinctFrom},
                                                                                {"NotDistinctFrom", ExpressionType::kNotDistinctFrom},
                                                                                {"Placeholder", ExpressionType::kPlaceholder},
                                                                                {"Predicate", ExpressionType::kPredicate},
                                                                                {"Raw", ExpressionType::kRaw},
                                                                                {"FilterFullText", ExpressionType::kFilterFullText},
                                                                                {"Unnest", ExpressionType::kUnnest}};

    auto it = str_to_type.find(str);
    if (it != str_to_type.end()) {
        return it->second;
    }

    return ExpressionType::kInvalid;
}

} // namespace infinity