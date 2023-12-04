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

import stl;
import column_vector;

import infinity_exception;
import bitmask;
import bitmask_buffer;

export module unary_operator;

namespace infinity {

export class UnaryOperator {
public:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr, bool nullable) {
        const auto *input_ptr = (const InputType *)(input->data_ptr_);
        const SharedPtr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (ResultType *)(result->data_ptr_);
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                Error<TypeException>("Invalid column vector type.");
            }
            case ColumnVectorType::kFlat: {
                Assert<TypeException>(result->vector_type() == ColumnVectorType::kFlat, "Target vector type isn't flat.");
                if (nullable) {
                    ExecuteFlatWithNull<InputType, ResultType, Operator>(input_ptr, input_null, result_ptr, result_null, count, state_ptr);

                } else {
                    ExecuteFlat<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
                }
                // Result tail_index need to update.
                result->tail_index_ = count;
                return;
            }
            case ColumnVectorType::kConstant: {
                Assert<TypeException>(count == 1, "Attempting to execute more than one row of the constant column vector.");
                if (nullable) {
                    if (input->nulls_ptr_->IsAllTrue()) {
                        result_null->SetAllTrue();
                        Operator::template Execute<InputType, ResultType>(input_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
                    } else {
                        result_null->SetFalse(0);
                    }
                } else {
                    Operator::template Execute<InputType, ResultType>(input_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
                }
                result->tail_index_ = 1;
                return;
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
            }
        }

        Error<TypeException>("Unexpected error.");
    }

private:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlat(const InputType *__restrict input_ptr,
                                   ResultType *__restrict result_ptr,
                                   SharedPtr<Bitmask> &result_null,
                                   SizeT count,
                                   void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), i, state_ptr);
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlatWithNull(const InputType *__restrict input_ptr,
                                           const SharedPtr<Bitmask> &input_null,
                                           ResultType *__restrict result_ptr,
                                           SharedPtr<Bitmask> &result_null,
                                           SizeT count,
                                           void *state_ptr) {
        if (input_null->IsAllTrue()) {
            // Initialized all true to output null bitmask.
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), i, state_ptr);
            }
        } else {
            result_null->DeepCopy(*input_null);

            const u64 *input_null_data = input_null->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
                if (input_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while (start_index < end_index) {
                        // Bug here? input_ptr[i] should be input_ptr[start_index]?
                        Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), start_index++, state_ptr);
                    }
                } else if (input_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    ;
                } else {
                    SizeT original_start = start_index;
                    while (start_index < end_index) {
                        if (input_null->IsTrue(start_index - original_start)) {
                            // This row isn't null
                            // Bug here? input_ptr[i] should be input_ptr[start_index]?
                            Operator::template Execute<InputType, ResultType>(input_ptr[i],
                                                                              result_ptr[i],
                                                                              result_null.get(),
                                                                              start_index++,
                                                                              state_ptr);
                        }
                    }
                }
            }
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const InputType *__restrict input_ptr,
                                            ResultType *__restrict result_ptr,
                                            SharedPtr<Bitmask> &result_null,
                                            SizeT count,
                                            void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), i, state_ptr);
        }
    }
};

} // namespace infinity
