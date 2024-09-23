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

export module filter_expression_push_down_helper;

import stl;
import base_expression;
import value;
import internal_types;

namespace infinity {

export enum class FilterCompareType : i8 { kEqual, kLess, kLessEqual, kGreater, kGreaterEqual, kAlwaysFalse, kAlwaysTrue, kInvalid };

export class FilterExpressionPushDownHelper {
public:
    static Value CalcValueResult(const SharedPtr<BaseExpression> &expression);

    static Tuple<ColumnID, Value, FilterCompareType>
    UnwindCast(const SharedPtr<BaseExpression> &cast_expr, Value &&right_val, FilterCompareType compare_type);
};

} // namespace infinity