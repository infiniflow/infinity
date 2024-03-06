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

export module secondary_index_scan_middle_expression;

import stl;
import base_expression;
import expression_type;
import value;
import filter_expression_push_down_helper;

namespace infinity {

// TODO: now do not support "NOT" expression and "!=" compare function in index scan

export enum class BooleanCombineType : i8 { kAnd, kOr, kInvalid };

export using FilterEvaluatorElem = std::variant<ColumnID, Value, FilterCompareType, BooleanCombineType>;

export Vector<FilterEvaluatorElem> BuildSecondaryIndexScanMiddleCommand(SharedPtr<BaseExpression> &index_filter_qualified_);

} // namespace infinity