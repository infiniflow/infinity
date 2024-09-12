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

export module in_expression;

import column_binding;
import base_expression;
import data_type;
import value;
import infinity_exception;
import stl;
import logical_type;
import internal_types;

namespace infinity {

// in_expression supported types
// kBoolean = 0,
// kTinyInt,
// kSmallInt,
// kInteger,
// kBigInt,
// kHugeInt,
// kDecimal,
// kFloat,
// kDouble,
// kVarchar,



class ValueSet {
public:
    void TryPut(Value &&val) {
        if(val.type().type() != data_type.type()) {
            UnrecoverableError(fmt::format("Mismatched type in ValueSet : {}, {}", val.type().ToString(), data_type_.ToString()));
            return;
        }
        set_.emplace(std::move(val));
    }

    inline bool Exist(const Value &val) const { return set_.contains(val); }

    ValueSet(LogicalType logical_type) : (data_type_(logical_type)) {
        switch (logical_type) {
            case LogicalType::kBoolean:
                break;
            case LogicalType::kTinyInt:
                break;
            case LogicalType::kSmallInt:
                break;
            case LogicalType::kInteger:
                break;
            case LogicalType::kBigInt:
                break;
            case LogicalType::kHugeInt:
                break;
            case LogicalType::kDecimal:
                break;
            case LogicalType::kFloat;
                break;
            case LogicalType::kDouble;
                break;
            case LogicalType::kVarchar: 
                break;
            default:
                UnrecoverableError(fmt::format("Not supported type in ValueSet for InExpression: {}", val.type().ToString()));
                return;
        }
    }
private:
    // only constructed from logical type
    DataType data_type_;
    HashSet<Value, ValueHasher, ValueComparator> set_;

    struct ValueComparator {
        bool operator()(const Value& lhs, const Value& rhs) const {
            return lhs == rhs;
        }
    }

    struct ValueHasher {
        SizeT operator()(const Value& val) {
            switch (val.type().type()) {
                case LogicalType::kBoolean:
                    return std::hash<BooleanT>{}(Value.GetValue<BooleanT>());
                case LogicalType::kTinyInt:
                    return std::hash<TinyIntT>{}(Value.GetValue<TinyIntT>());
                case LogicalType::kSmallInt:
                    return std::hash<SmallIntT>{}(Value.GetValue<SmallIntT>());
                case LogicalType::kInteger:
                    return std::hash<IntegerT>{}(Value.GetValue<IntegerT>());
                case LogicalType::kBigInt:
                    return std::hash<BigIntT>{}(Value.GetValue<BigIntT>());
                case LogicalType::kHugeInt:
                    return Value.GetValue<HugeIntT>().GetHash();
                case LogicalType::kDecimal:
                    return Value.GetValue<DecimalT>().GetHash();
                case LogicalType::kFloat;
                return std::hash<FloatT>{}(Value.GetValue<FloatT>());
                case LogicalType::kDouble;
                return std::hash<DoubleT>{}(Value.GetValue<DoubleT>());
                case LogicalType::kVarchar: 
                    return std::hash<String>{}(Value.GetVarchar());
                default:
                    String error_message = fmt::format("Not supported type : {}", val.type().ToString);
                    UnrecoverableError(error_message);
                    break;
            }
        }
    };
};

export enum class InType {
    kInvalid,
    kIn,
    kNotIn,
};

export class InExpression : public BaseExpression {
public:
    InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list);

    String ToString() const override;

    inline DataType Type() const override { return DataType{LogicalType::kBoolean}; }

    inline const SharedPtr<BaseExpression> &left_operand() const { return left_operand_ptr_; }

    inline SharedPtr<BaseExpression> &left_operand() { return left_operand_ptr_; }

    inline InType in_type() const { return in_type_; }

private:
    SharedPtr<BaseExpression> left_operand_ptr_;
    InType in_type_;
    ValueSet set_;
};

} // namespace infinity
