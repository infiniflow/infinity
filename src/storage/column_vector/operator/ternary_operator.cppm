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

export module ternary_operator;

import stl;
import column_vector;
import logger;
import infinity_exception;
import bitmask;
import bitmask_buffer;

namespace infinity {

export class TernaryOperator {
public:
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &first,
                               const SharedPtr<ColumnVector> &second,
                               const SharedPtr<ColumnVector> &third,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {

        const auto *first_ptr = (const FirstType *)(first->data());
        const SharedPtr<Bitmask> &first_null = first->nulls_ptr_;

        const auto *second_ptr = (const SecondType *)(second->data());
        const SharedPtr<Bitmask> &second_null = second->nulls_ptr_;

        const auto *third_ptr = (const ThirdType *)(third->data());
        const SharedPtr<Bitmask> &third_null = second->nulls_ptr_;

        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        // 8 cases for first/second/third
        // Flat Flat Flat
        // Flat Flat Constant
        // Flat Constant Flat
        // Flat Constant Constant
        // Constant Flat Flat
        // Constant Flat Constant
        // Constant Constant Flat
        // Constant Constant Constant

        // 1. Flat Flat Flat
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kFlat) {
            if (nullable) {
                ExecuteFFFWithNull<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                           first_null,
                                                                                           second_ptr,
                                                                                           second_null,
                                                                                           third_ptr,
                                                                                           third_null,
                                                                                           result_ptr,
                                                                                           result_null,
                                                                                           count,
                                                                                           state_ptr);
            } else {
                ExecuteFFF<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                   second_ptr,
                                                                                   third_ptr,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);
            }
            result->Finalize(count);
        }

        // 2. Flat Flat Constant
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kConstant) {
            String error_message = "Not support";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // 3. Flat Constant Flat
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // 4. Flat Constant Constant
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kConstant) {
            if (nullable) {
                ExecuteFCCWithNull<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                           first_null,
                                                                                           second_ptr,
                                                                                           second_null,
                                                                                           third_ptr,
                                                                                           third_null,
                                                                                           result_ptr,
                                                                                           result_null,
                                                                                           count,
                                                                                           state_ptr);
            } else {
                ExecuteFCC<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                   second_ptr,
                                                                                   third_ptr,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);
            }
            result->Finalize(count);
        }

        // 5. Constant Flat Flat
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // 6. Constant Flat Constant
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kConstant) {
            String error_message = "Not support";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // 7. Constant Constant Flat
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        // 8. Constant Constant Constant
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kConstant) {
            // All input are constant

            if (nullable) {
                // Check if all input is true
                if (first_null->IsAllTrue() && second_null->IsAllTrue() && third_null->IsAllTrue()) {
                    result_null->SetAllTrue();
                    Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[0],
                                                                                             second_ptr[0],
                                                                                             third_ptr[0],
                                                                                             result_ptr[0],
                                                                                             result_null.get(),
                                                                                             0,
                                                                                             state_ptr);
                } else {
                    result->nulls_ptr_->SetFalse(0);
                }
            } else {
                // Not check nullable
                Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[0],
                                                                                         second_ptr[0],
                                                                                         third_ptr[0],
                                                                                         result_ptr[0],
                                                                                         result_null.get(),
                                                                                         0,
                                                                                         state_ptr);
            }
            result->Finalize(1);
            return;
        }
    }

private:
    // 1. Flat Flat Flat
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline ExecuteFFF(const FirstType *__restrict first_ptr,
                                  const SecondType *__restrict second_ptr,
                                  const ThirdType *__restrict third_ptr,
                                  ResultType *__restrict result_ptr,
                                  SharedPtr<Bitmask> &result_null,
                                  SizeT count,
                                  void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[i],
                                                                                     third_ptr[i],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr);
        }
    }

    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline ExecuteFFFWithNull(const FirstType *__restrict first_ptr,
                                          const SharedPtr<Bitmask> &first_null,
                                          const SecondType *__restrict second_ptr,
                                          const SharedPtr<Bitmask> &second_null,
                                          const ThirdType *__restrict third_ptr,
                                          const SharedPtr<Bitmask> &third_null,
                                          ResultType *__restrict result_ptr,
                                          SharedPtr<Bitmask> &result_null,
                                          SizeT count,
                                          void *state_ptr) {

        if (first_null->IsAllTrue() && second_null->IsAllTrue() && third_null->IsAllTrue()) {
            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                         second_ptr[i],
                                                                                         third_ptr[i],
                                                                                         result_ptr[i],
                                                                                         result_null.get(),
                                                                                         i,
                                                                                         state_ptr);
            }
        } else {
            result_null->DeepCopy(*first_null);
            result_null->Merge(*second_null);
            result_null->Merge(*third_null);

            const u64 *result_null_data = result_null->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
                if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while (start_index < end_index) {
                        Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[start_index],
                                                                                                 second_ptr[start_index],
                                                                                                 third_ptr[start_index],
                                                                                                 result_ptr[start_index],
                                                                                                 result_null.get(),
                                                                                                 start_index,
                                                                                                 state_ptr);
                        ++start_index;
                    }
                } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    ;
                } else {
                    SizeT original_start = start_index;
                    while (start_index < end_index) {
                        if (result_null->IsTrue(start_index - original_start)) {
                            // This row isn't null
                            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[start_index],
                                                                                                     second_ptr[start_index],
                                                                                                     third_ptr[start_index],
                                                                                                     result_ptr[start_index],
                                                                                                     result_null.get(),
                                                                                                     start_index,
                                                                                                     state_ptr);
                            ++start_index;
                        }
                    }
                }
            }
        }
    }

    // 2. Flat Constant Constant
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline ExecuteFCC(const FirstType *__restrict first_ptr,
                                  const SecondType *__restrict second_ptr,
                                  const ThirdType *__restrict third_ptr,
                                  ResultType *__restrict result_ptr,
                                  SharedPtr<Bitmask> &result_null,
                                  SizeT count,
                                  void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[0],
                                                                                     third_ptr[0],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr);
        }
    }

    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline ExecuteFCCWithNull(const FirstType *__restrict first_ptr,
                                          const SharedPtr<Bitmask> &first_null,
                                          const SecondType *__restrict second_ptr,
                                          const SharedPtr<Bitmask> &second_null,
                                          const ThirdType *__restrict third_ptr,
                                          const SharedPtr<Bitmask> &third_null,
                                          ResultType *__restrict result_ptr,
                                          SharedPtr<Bitmask> &result_null,
                                          SizeT count,
                                          void *state_ptr) {

        if (first_null->IsAllTrue() && second_null->IsAllTrue() && third_null->IsAllTrue()) {
            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                         second_ptr[0],
                                                                                         third_ptr[0],
                                                                                         result_ptr[i],
                                                                                         result_null.get(),
                                                                                         i,
                                                                                         state_ptr);
            }
        } else {
            result_null->DeepCopy(*first_null);
            result_null->Merge(*second_null);
            result_null->Merge(*third_null);

            const u64 *result_null_data = result_null->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
                if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while (start_index < end_index) {
                        Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[start_index],
                                                                                                 second_ptr[0],
                                                                                                 third_ptr[0],
                                                                                                 result_ptr[start_index],
                                                                                                 result_null.get(),
                                                                                                 start_index,
                                                                                                 state_ptr);
                        ++start_index;
                    }
                } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    ;
                } else {
                    SizeT original_start = start_index;
                    while (start_index < end_index) {
                        if (result_null->IsTrue(start_index - original_start)) {
                            // This row isn't null
                            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[start_index],
                                                                                                     second_ptr[0],
                                                                                                     third_ptr[0],
                                                                                                     result_ptr[start_index],
                                                                                                     result_null.get(),
                                                                                                     start_index,
                                                                                                     state_ptr);
                            ++start_index;
                        }
                    }
                }
            }
        }
    }
};

} // namespace infinity
