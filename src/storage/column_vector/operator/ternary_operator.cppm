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

export module infinity_core:ternary_operator;

import :column_vector;
import :logger;
import :infinity_exception;
import :roaring_bitmap;

namespace infinity {

template <typename FirstType, typename SecondType, typename ThirdType, typename Operator>
class BooleanResultTernaryOperator {
public:
    static void inline Execute(const std::shared_ptr<ColumnVector> &first,
                               const std::shared_ptr<ColumnVector> &second,
                               const std::shared_ptr<ColumnVector> &third,
                               std::shared_ptr<ColumnVector> &result,
                               size_t count,
                               void *state_ptr_first,
                               void *state_ptr,
                               bool nullable) {

        auto first_vector_type = first->vector_type();
        auto second_vector_type = second->vector_type();
        auto third_vector_type = third->vector_type();

        // Only support: Flat, Constant, Constant (for LIKE operator)
        if (first_vector_type == ColumnVectorType::kFlat && second_vector_type == ColumnVectorType::kConstant &&
            third_vector_type == ColumnVectorType::kConstant) {
            ExecuteFlatConstantConstant(first, second, third, result, count, nullable);
        } else {
            // Helper lambda to convert ColumnVectorType to string
            auto type_to_string = [](ColumnVectorType type) -> std::string {
                switch (type) {
                    case ColumnVectorType::kFlat:
                        return "kFlat";
                    case ColumnVectorType::kConstant:
                        return "kConstant";
                    case ColumnVectorType::kCompactBit:
                        return "kCompactBit";
                    case ColumnVectorType::kHeterogeneous:
                        return "kHeterogeneous";
                    default:
                        return fmt::format("Invalid({})", int(type));
                }
            };
            std::string types =
                fmt::format("({}, {}, {})", type_to_string(first_vector_type), type_to_string(second_vector_type), type_to_string(third_vector_type));
            UnrecoverableError(fmt::format("Not support ternary functions with column vector types {}", types));
        }
    }

private:
    static void inline ExecuteFlatConstantConstant(const std::shared_ptr<ColumnVector> &first,
                                                   const std::shared_ptr<ColumnVector> &second,
                                                   const std::shared_ptr<ColumnVector> &third,
                                                   std::shared_ptr<ColumnVector> &result,
                                                   size_t count,
                                                   bool nullable) {
        const auto &first_null = first->nulls_ptr_;
        const auto &second_null = second->nulls_ptr_;
        const auto &third_null = third->nulls_ptr_;
        auto &result_null = result->nulls_ptr_;

        auto second_c = ColumnValueReader<SecondType>(second)[0];
        auto third_c = ColumnValueReader<ThirdType>(third)[0];
        if (nullable && !(second_null->IsAllTrue() && third_null->IsAllTrue())) {
            result_null->SetAllFalse();
        } else if (!nullable || first_null->IsAllTrue()) {
            result_null->SetAllTrue();
            auto first_ptr = ColumnValueReader<FirstType>(first);
            BooleanColumnWriter result_ptr(result);
            for (size_t i = 0; i < count; ++i) {
                Operator::Execute(first_ptr[i], second_c, third_c, result_ptr[i], result_null.get(), 0, nullptr, nullptr, nullptr);
            }
        } else {
            *result_null = *first_null;
            auto first_ptr = ColumnValueReader<FirstType>(first);
            BooleanColumnWriter result_ptr(result);
            result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
                if (row_index >= count) {
                    return false;
                }
                Operator::Execute(first_ptr[row_index],
                                  second_c,
                                  third_c,
                                  result_ptr[row_index],
                                  result_null.get(),
                                  row_index,
                                  nullptr,
                                  nullptr,
                                  nullptr);
                return row_index + 1 < count;
            });
        }
        result->Finalize(count);
    }
};

export class TernaryOperator {
public:
    // case for ternary operator which returns BooleanT result
    template <typename FirstType, typename SecondType, typename ThirdType, std::same_as<BooleanT> ResultType, typename Operator>
    static void inline Execute(const std::shared_ptr<ColumnVector> &first,
                               const std::shared_ptr<ColumnVector> &second,
                               const std::shared_ptr<ColumnVector> &third,
                               std::shared_ptr<ColumnVector> &result,
                               size_t count,
                               void *state_ptr_first,
                               void *state_ptr,
                               bool nullable) {
        return BooleanResultTernaryOperator<FirstType, SecondType, ThirdType, Operator>::Execute(first,
                                                                                                 second,
                                                                                                 third,
                                                                                                 result,
                                                                                                 count,
                                                                                                 state_ptr_first,
                                                                                                 state_ptr,
                                                                                                 nullable);
    }

    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operator>
    static void inline Execute(const std::shared_ptr<ColumnVector> &first,
                               const std::shared_ptr<ColumnVector> &second,
                               const std::shared_ptr<ColumnVector> &third,
                               std::shared_ptr<ColumnVector> &result,
                               size_t count,
                               void *state_ptr_first,
                               void *state_ptr,
                               bool nullable) {

        const auto *first_ptr = (const FirstType *)(first->data().get());
        const std::shared_ptr<Bitmask> &first_null = first->nulls_ptr_;

        const auto *second_ptr = (const SecondType *)(second->data()).get();
        const std::shared_ptr<Bitmask> &second_null = second->nulls_ptr_;

        const auto *third_ptr = (const ThirdType *)(third->data().get());
        const std::shared_ptr<Bitmask> &third_null = second->nulls_ptr_;

        auto *result_ptr = (ResultType *)(result->data().get());
        std::shared_ptr<Bitmask> &result_null = result->nulls_ptr_;

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
            UnrecoverableError("Not support");
        }

        // 3. Flat Constant Flat
        if (first->vector_type() == ColumnVectorType::kFlat && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            UnrecoverableError("Not support");
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
            UnrecoverableError("Not support");
        }

        // 6. Constant Flat Constant
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kFlat &&
            third->vector_type() == ColumnVectorType::kConstant) {
            UnrecoverableError("Not support");
        }

        // 7. Constant Constant Flat
        if (first->vector_type() == ColumnVectorType::kConstant && second->vector_type() == ColumnVectorType::kConstant &&
            third->vector_type() == ColumnVectorType::kFlat) {
            UnrecoverableError("Not support");
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
                                  std::shared_ptr<Bitmask> &result_null,
                                  size_t count,
                                  void *state_ptr_first,
                                  void *state_ptr) {
        for (size_t i = 0; i < count; i++) {
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
                                          const std::shared_ptr<Bitmask> &first_null,
                                          const SecondType *__restrict second_ptr,
                                          const std::shared_ptr<Bitmask> &second_null,
                                          const ThirdType *__restrict third_ptr,
                                          const std::shared_ptr<Bitmask> &third_null,
                                          ResultType *__restrict result_ptr,
                                          std::shared_ptr<Bitmask> &result_null,
                                          size_t count,
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
                                  std::shared_ptr<Bitmask> &result_null,
                                  size_t count,
                                  void *state_ptr_first,
                                  void *state_ptr) {
        for (size_t i = 0; i < count; i++) {
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
                                          const std::shared_ptr<Bitmask> &first_null,
                                          const SecondType *__restrict second_ptr,
                                          const std::shared_ptr<Bitmask> &second_null,
                                          const ThirdType *__restrict third_ptr,
                                          const std::shared_ptr<Bitmask> &third_null,
                                          ResultType *__restrict result_ptr,
                                          std::shared_ptr<Bitmask> &result_null,
                                          size_t count,
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
