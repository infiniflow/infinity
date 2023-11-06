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

import parser;
import base_expression;

import infinity_exception;
import bound_cast_func;
import stl;
import third_party;
import cast_function;

module cast_expression;

namespace infinity {

SharedPtr<BaseExpression> CastExpression::AddCastToType(const SharedPtr<BaseExpression> &source_expr_ptr, const DataType &target_type) {
    if (source_expr_ptr->Type() == target_type) {
        return source_expr_ptr;
    }

    if (CastExpression::CanCast(source_expr_ptr->Type(), target_type)) {
        BoundCastFunc cast = CastFunction::GetBoundFunc(source_expr_ptr->Type(), target_type);
        return MakeShared<CastExpression>(cast, source_expr_ptr, target_type);
    } else {
        Error<PlannerException>(Format("Can't cast from: {} to {}", source_expr_ptr->Type().ToString(), target_type.ToString()));
    }
}

bool CastExpression::CanCast(const DataType &source, const DataType &target) {
    switch (target.type()) {
        case LogicalType::kNull:
        case LogicalType::kInvalid:
            Error<PlannerException>("Invalid data type");
        default:;
    }

    switch (source.type()) {
        case LogicalType::kBoolean:
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kFloat:
        case LogicalType::kDouble:
        case LogicalType::kDecimal:
            switch (target.type()) {
                case LogicalType::kBoolean:
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger:
                case LogicalType::kBigInt:
                case LogicalType::kFloat:
                case LogicalType::kDouble:
                case LogicalType::kDecimal:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kDate:
            switch (target.type()) {
                case LogicalType::kDate:
                case LogicalType::kDateTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kTime:
            switch (target.type()) {
                case LogicalType::kTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kDateTime:
            switch (target.type()) {
                case LogicalType::kDate:
                case LogicalType::kDateTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kInterval:
            switch (target.type()) {
                case LogicalType::kInterval:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kVarchar:
            switch (target.type()) {
                case LogicalType::kBoolean:
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger:
                case LogicalType::kBigInt:
                case LogicalType::kFloat:
                case LogicalType::kDouble:
                case LogicalType::kDecimal:
                case LogicalType::kDate:
                case LogicalType::kTime:
                case LogicalType::kDateTime:
                case LogicalType::kInterval:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        default: {
            Error<PlannerException>("Invalid data type");
        }
    }
}

String CastExpression::ToString() const { return Format("Cast({} AS {})", arguments_[0]->Name(), target_type_.ToString()); }

} // namespace infinity
