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
import roaring_bitmap;

namespace infinity {

export class TernaryOperator {
public:
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &first,
                               const SharedPtr<ColumnVector> &second,
                               const SharedPtr<ColumnVector> &third,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr_first,
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
                                                                                           state_ptr_first,
                                                                                           state_ptr);
            } else {
                ExecuteFFF<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                   second_ptr,
                                                                                   third_ptr,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr_first,
                                                                                   state_ptr);
            }
            result->Finalize(count);
        }

        // 2. Flat Flat Constant
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kConstant) {
            String error_message = "Not support";
            UnrecoverableError(error_message);
        }

        // 3. Flat Constant Flat
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
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
                                                                                           state_ptr_first,
                                                                                           state_ptr);
            } else {
                ExecuteFCC<FirstType, SecondType, ThirdType, ResultType, Operator>(first_ptr,
                                                                                   second_ptr,
                                                                                   third_ptr,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr_first,
                                                                                   state_ptr);
            }
            result->Finalize(count);
        }

        // 5. Constant Flat Flat
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
            UnrecoverableError(error_message);
        }

        // 6. Constant Flat Constant
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kConstant) {
            String error_message = "Not support";
            UnrecoverableError(error_message);
        }

        // 7. Constant Constant Flat
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            String error_message = "Not support";
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
                                                                                             state_ptr_first,
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
                                                                                         state_ptr_first,
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
                                  void *state_ptr_first,
                                  void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[i],
                                                                                     third_ptr[i],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr_first,
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
                                          void *state_ptr_first,
                                          void *state_ptr) {
        *result_null = *first_null;
        result_null->MergeAnd(*second_null);
        result_null->MergeAnd(*third_null);
        result_null->RoaringBitmapApplyFunc([&](u32 i) -> bool {
            if (i >= count) {
                return false;
            }
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[i],
                                                                                     third_ptr[i],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr_first,
                                                                                     state_ptr);
            return i + 1 < count;
        });
    }

    // 2. Flat Constant Constant
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline ExecuteFCC(const FirstType *__restrict first_ptr,
                                  const SecondType *__restrict second_ptr,
                                  const ThirdType *__restrict third_ptr,
                                  ResultType *__restrict result_ptr,
                                  SharedPtr<Bitmask> &result_null,
                                  SizeT count,
                                  void *state_ptr_first,
                                  void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[0],
                                                                                     third_ptr[0],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr_first,
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
                                          void *state_ptr_first,
                                          void *state_ptr) {
        *result_null = *first_null;
        if (!(second_null->IsAllTrue() && third_null->IsAllTrue())) {
            result_null->SetAllFalse();
        }
        result_null->RoaringBitmapApplyFunc([&](u32 i) -> bool {
            if (i >= count) {
                return false;
            }
            Operator::template Execute<FirstType, SecondType, ThirdType, ResultType>(first_ptr[i],
                                                                                     second_ptr[0],
                                                                                     third_ptr[0],
                                                                                     result_ptr[i],
                                                                                     result_null.get(),
                                                                                     i,
                                                                                     state_ptr_first,
                                                                                     state_ptr);
            return i + 1 < count;
        });
    }
};

} // namespace infinity
