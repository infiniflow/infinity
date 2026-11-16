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

export module infinity_core:unary_operator;

import :column_vector;
import :logger;
import :infinity_exception;
import :roaring_bitmap;

import std;

import internal_types;

namespace infinity {

struct ColumnVectorCastData;

export class UnaryOperator {
public:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline Execute(const std::shared_ptr<ColumnVector> &input,
                               std::shared_ptr<ColumnVector> &result,
                               size_t count,
                               void *state_ptr_input,
                               void *state_ptr,
                               bool nullable) {
        const auto *input_ptr = (const InputType *)(input->data());
        const std::shared_ptr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (ResultType *)(result->data());
        std::shared_ptr<Bitmask> &result_null = result->nulls_ptr_;

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                UnrecoverableError("Invalid column vector type.");
            }
            case ColumnVectorType::kCompactBit: {
                // A compact-bit input reaches this branch with a non-boolean
                // result when a boolean column is cast to another type, e.g.
                // CAST(bool_col AS VARCHAR). The bit-packed helpers below
                // produce booleans, so read the input element by element and
                // let the operator write the real result type. The old code
                // rejected that combination with UnrecoverableError, which
                // aborted the whole server on an ordinary SELECT.
                if constexpr (!std::is_same_v<std::remove_cv_t<InputType>, BooleanT>) {
                    UnrecoverableError("kCompactBit input should be BooleanT.");
                } else if constexpr (!std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    if (result->vector_type() != ColumnVectorType::kFlat) {
                        UnrecoverableError("Casting a kCompactBit input needs a flat result.");
                    }
                    for (size_t i = 0; i < count; ++i) {
                        if (nullable && !input_null->IsTrue(i)) {
                            result_null->SetFalse(i);
                            continue;
                        }
                        Operator::template Execute<BooleanT, ResultType>(input->buffer_->GetCompactBit(i),
                                                                         result_ptr[i],
                                                                         result_null.get(),
                                                                         i,
                                                                         state_ptr_input,
                                                                         state_ptr);
                    }
                    result->Finalize(count);
                    return;
                }
                if (result->vector_type() != ColumnVectorType::kCompactBit) {
                    UnrecoverableError("Target vector type isn't kCompactBit.");
                }
                if (nullable && !(input_null->IsAllTrue())) {
                    ExecuteBooleanWithNull<Operator>(input, result, count, state_ptr_input, state_ptr);
                } else {
                    ExecuteBoolean<Operator>(input, result, count, state_ptr_input, state_ptr);
                }
                // Result tail_index need to update.
                result->Finalize(count);
                return;
            }
            case ColumnVectorType::kFlat: {
                // if (result->vector_type() != ColumnVectorType::kFlat) {
                //     UnrecoverableError("Target vector type isn't flat.");
                // }
                if constexpr (std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    if (nullable)
                        ExecuteFlatToBooleanWithNull<InputType, ResultType, Operator>(input_ptr, result, count, state_ptr_input, state_ptr);
                    else
                        ExecuteFlatToBoolean<InputType, ResultType, Operator>(input_ptr, result, count, state_ptr_input, state_ptr);
                } else {
                    if (nullable) {
                        ExecuteFlatWithNull<InputType, ResultType, Operator>(input_ptr,
                                                                             input_null,
                                                                             result_ptr,
                                                                             result_null,
                                                                             count,
                                                                             state_ptr_input,
                                                                             state_ptr);

                    } else {
                        ExecuteFlat<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr_input, state_ptr);
                    }
                }
                // Result tail_index need to update.
                result->Finalize(count);
                return;
            }
            case ColumnVectorType::kConstant: {
                if (nullable && !(input_null->IsAllTrue())) {
                    result_null->SetFalse(0);
                } else {
                    result_null->SetAllTrue();
                    if constexpr (std::is_same_v<InputType, BooleanT> and std::is_same_v<ResultType, BooleanT>) {
                        BooleanT result_value;
                        Operator::Execute(input->buffer_->GetCompactBit(0), result_value, result_null.get(), 0, state_ptr_input, state_ptr);
                        result->buffer_->SetCompactBit(0, result_value);
                    } else if constexpr (std::is_same_v<ResultType, BooleanT>) {
                        BooleanT result_value;
                        Operator::Execute(input_ptr[0], result_value, result_null.get(), 0, state_ptr_input, state_ptr);
                        result->buffer_->SetCompactBit(0, result_value);
                    } else if constexpr (std::is_same_v<InputType, EmbeddingT>) {
                        EmbeddingT embedding_input(input->data(), false);
                        Operator::template Execute<InputType, ResultType>(embedding_input,
                                                                          result_ptr[0],
                                                                          result_null.get(),
                                                                          0,
                                                                          state_ptr_input,
                                                                          state_ptr);
                        embedding_input.Reset();
                    } else {
                        Operator::template Execute<InputType, ResultType>(input_ptr[0],
                                                                          result_ptr[0],
                                                                          result_null.get(),
                                                                          0,
                                                                          state_ptr_input,
                                                                          state_ptr);
                    }
                }
                // A constant vector represents the same value for all rows. The
                // operation is applied once on index 0 and finalize broadcasts the
                // result to count rows (count may be > 1, e.g. when a literal
                // constant is cast within a multi-row expression evaluation).
                result->Finalize(count);
                return;
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr_input, state_ptr);
            }
        }
        UnrecoverableError("Unexpected error.");
    }

private:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlat(const InputType *__restrict input_ptr,
                                   ResultType *__restrict result_ptr,
                                   std::shared_ptr<Bitmask> &result_null,
                                   size_t count,
                                   void *state_ptr_input,
                                   void *state_ptr) {
        for (size_t i = 0; i < count; i++) {
            Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), i, state_ptr_input, state_ptr);
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlatWithNull(const InputType *__restrict input_ptr,
                                           const std::shared_ptr<Bitmask> &input_null,
                                           ResultType *__restrict result_ptr,
                                           std::shared_ptr<Bitmask> &result_null,
                                           size_t count,
                                           void *state_ptr_input,
                                           void *state_ptr) {
        *result_null = *input_null;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
            if (row_index >= count) {
                return false;
            }
            Operator::template Execute<InputType, ResultType>(input_ptr[row_index],
                                                              result_ptr[row_index],
                                                              result_null.get(),
                                                              row_index,
                                                              state_ptr_input,
                                                              state_ptr);
            return row_index + 1 < count;
        });
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const InputType *__restrict input_ptr,
                                            ResultType *__restrict result_ptr,
                                            std::shared_ptr<Bitmask> &result_null,
                                            size_t count,
                                            void *state_ptr_input,
                                            void *state_ptr) {
        for (size_t i = 0; i < count; i++) {
            Operator::template Execute<InputType, ResultType>(input_ptr[i], result_ptr[i], result_null.get(), i, state_ptr_input, state_ptr);
        }
    }

    template <typename Operator>
    static void inline ExecuteBoolean(const std::shared_ptr<ColumnVector> &input,
                                      std::shared_ptr<ColumnVector> &result,
                                      size_t count,
                                      void *state_ptr_input,
                                      void *state_ptr) {
        std::shared_ptr<Bitmask> &result_null = result->nulls_ptr_;
        result_null->SetAllTrue();
        size_t count_bytes = count / 8;
        size_t count_tail = count % 8;
        auto input_u8 = reinterpret_cast<const u8 *>(input->data());
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        for (size_t i = 0; i < count_bytes; ++i) {
            Operator::Execute(input_u8[i], result_u8[i], result_null.get(), 0, state_ptr_input, state_ptr);
        }
        if (count_tail > 0) {
            u8 &tail_u8 = result_u8[count_bytes];
            u8 ans;
            Operator::Execute(input_u8[count_bytes], ans, result_null.get(), 0, state_ptr_input, state_ptr);
            u8 keep_mask = u8(0xff) << count_tail;
            tail_u8 = (tail_u8 & keep_mask) | (ans & ~keep_mask);
        }
    }

    template <typename Operator>
    static void inline ExecuteBooleanWithNull(const std::shared_ptr<ColumnVector> &input,
                                              std::shared_ptr<ColumnVector> &result,
                                              size_t count,
                                              void *state_ptr_input,
                                              void *state_ptr) {
        const auto &input_null = input->nulls_ptr_;
        auto &result_null = result->nulls_ptr_;
        *result_null = *input_null;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
            if (row_index >= count) {
                return false;
            }
            // This row isn't null
            BooleanT answer;
            Operator::Execute(input->buffer_->GetCompactBit(row_index), answer, result_null.get(), row_index, state_ptr_input, state_ptr);
            result->buffer_->SetCompactBit(row_index, answer);
            return row_index + 1 < count;
        });
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlatToBoolean(const InputType *__restrict input_ptr,
                                            std::shared_ptr<ColumnVector> &result,
                                            size_t count,
                                            void *state_ptr_input,
                                            void *state_ptr) {
        for (size_t i = 0; i < count; i++) {
            BooleanT answer;
            Operator::Execute(input_ptr[i], answer, result->nulls_ptr_.get(), i, state_ptr_input, state_ptr);
            result->buffer_->SetCompactBit(i, answer);
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline ExecuteFlatToBooleanWithNull(const InputType *__restrict input_ptr,
                                                    std::shared_ptr<ColumnVector> &result,
                                                    size_t count,
                                                    void *state_ptr_input,
                                                    void *state_ptr) {
        auto &result_null = result->nulls_ptr_;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
            if (row_index >= count) {
                return false;
            }
            BooleanT answer;
            Operator::Execute(input_ptr[row_index], answer, result_null.get(), row_index, state_ptr_input, state_ptr);
            result->buffer_->SetCompactBit(row_index, answer);
            return row_index + 1 < count;
        });
    }
};

} // namespace infinity
