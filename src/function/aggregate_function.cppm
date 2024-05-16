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

#include <type_traits>

export module aggregate_function;

import stl;
import function;
import function_data;
import column_vector;
import vector_buffer;
import infinity_exception;
import base_expression;
import data_type;
import logical_type;
import internal_types;

namespace infinity {

using AggregateInitializeFuncType = std::function<void(ptr_t)>;
using AggregateUpdateFuncType = std::function<void(ptr_t, const SharedPtr<ColumnVector> &)>;
using AggregateFinalizeFuncType = std::function<ptr_t(ptr_t)>;

class AggregateOperation {
public:
    template <typename AggregateState>
    static inline void StateInitialize(const ptr_t state) {
        ((AggregateState *)state)->Initialize();
    }

    template <typename AggregateState, typename InputType>
    static inline void StateUpdate(const ptr_t state, const SharedPtr<ColumnVector> &input_column_vector) {
        // Loop execute state update according to the input column vector

        switch (input_column_vector->vector_type()) {
            case ColumnVectorType::kCompactBit: {
                if constexpr (!std::is_same_v<InputType, BooleanT>) {
                    UnrecoverableError("kCompactBit column vector only support Boolean type");
                } else {
                    // only for count, min, max
                    SizeT row_count = input_column_vector->Size();
                    BooleanT value;
                    const VectorBuffer *buffer = input_column_vector->buffer_.get();
                    for (SizeT idx = 0; idx < row_count; ++idx) {
                        value = buffer->GetCompactBit(idx);
                        ((AggregateState *)state)->Update(&value, 0);
                    }
                }
                break;
            }
            case ColumnVectorType::kFlat: {
                SizeT row_count = input_column_vector->Size();
                auto *input_ptr = (InputType *)(input_column_vector->data());
                for (SizeT idx = 0; idx < row_count; ++idx) {
                    ((AggregateState *)state)->Update(input_ptr, idx);
                }
                break;
            }
            case ColumnVectorType::kConstant: {
                if (input_column_vector->data_type()->type() == LogicalType::kBoolean) {
                    if constexpr (!std::is_same_v<InputType, BooleanT>) {
                        UnrecoverableError("types do not match");
                    } else {
                        BooleanT value = input_column_vector->buffer_->GetCompactBit(0);
                        ((AggregateState *)state)->Update(&value, 0);
                    }
                    break;
                }
                auto *input_ptr = (InputType *)(input_column_vector->data());
                ((AggregateState *)state)->Update(input_ptr, 0);
                break;
            }
            case ColumnVectorType::kHeterogeneous: {
                UnrecoverableError("Not implement: Heterogeneous type");
            }
            default: {
                UnrecoverableError("Not implement: Other type");
            }
        }
    }

    template <typename AggregateState, typename ResultType>
    static inline ptr_t StateFinalize(const ptr_t state) {
        // Loop execute state update according to the input column vector
        ptr_t result = ((AggregateState *)state)->Finalize();
        return result;
    }
};

export class AggregateFunction : public Function {
public:
    explicit AggregateFunction(String name,
                               DataType argument_type,
                               DataType return_type,
                               SizeT state_size,
                               AggregateInitializeFuncType init_func,
                               AggregateUpdateFuncType update_func,
                               AggregateFinalizeFuncType finalize_func)
        : Function(std::move(name), FunctionType::kAggregate), init_func_(std::move(init_func)), update_func_(std::move(update_func)),
          finalize_func_(std::move(finalize_func)), argument_type_(std::move(argument_type)), return_type_(std::move(return_type)),
          state_size_(state_size) {}

    void CastArgumentTypes(BaseExpression &input_argument);

    [[nodiscard]] const DataType &return_type() const { return return_type_; }

    [[nodiscard]] String ToString() const override;

    UniquePtr<char[]> InitState() const { return MakeUnique<char[]>(state_size_); }

    [[nodiscard]] String GetFuncName() const { return name_; }

public:
    AggregateInitializeFuncType init_func_;
    AggregateUpdateFuncType update_func_;
    AggregateFinalizeFuncType finalize_func_;

    DataType argument_type_;
    DataType return_type_;

    SizeT state_size_{};
};

export template <typename AggregateState, typename InputType, typename ResultType>
inline AggregateFunction UnaryAggregate(const String &name, const DataType &input_type, const DataType &return_type) {
    return AggregateFunction(name,
                             input_type,
                             return_type,
                             AggregateState::Size(input_type),
                             AggregateOperation::StateInitialize<AggregateState>,
                             AggregateOperation::StateUpdate<AggregateState, InputType>,
                             AggregateOperation::StateFinalize<AggregateState, ResultType>);
}

} // namespace infinity
