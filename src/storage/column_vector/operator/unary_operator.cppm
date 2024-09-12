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

export module unary_operator;

import stl;
import column_vector;
import logger;
import infinity_exception;
import roaring_bitmap;
import internal_types;

namespace infinity {

struct ColumnVectorCastData;

export class UnaryOperator {
public:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr, bool nullable) {
        const auto *input_ptr = (const InputType *)(input->data());
        const SharedPtr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                String error_message = "Invalid column vector type.";
                UnrecoverableError(error_message);
            }
            case ColumnVectorType::kCompactBit: {
                if (result->vector_type() != ColumnVectorType::kCompactBit) {
                    String error_message = "Target vector type isn't kCompactBit.";
                    UnrecoverableError(error_message);
                }
                if constexpr (!std::is_same_v<std::remove_cv_t<InputType>, BooleanT> || !std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    String error_message = "kCompactBit should match with BooleanT.";
                    UnrecoverableError(error_message);
                }
                if (nullable && !(input_null->IsAllTrue())) {
                    ExecuteBooleanWithNull<Operator>(input, result, count, state_ptr);
                } else {
                    ExecuteBoolean<Operator>(input, result, count, state_ptr);
                }
                // Result tail_index need to update.
                result->Finalize(count);
                return;
            }
            case ColumnVectorType::kFlat: {
                if (result->vector_type() != ColumnVectorType::kFlat) {
                    String error_message = "Target vector type isn't flat.";
                    UnrecoverableError(error_message);
                }
                if constexpr (std::is_same_v<std::remove_cv_t<InputType>, BooleanT> || std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    String error_message = "BooleanT type should not be kFlat.";
                    UnrecoverableError(error_message);
                }
                if (nullable) {
                    ExecuteFlatWithNull<InputType, ResultType, Operator>(input_ptr, input_null, result_ptr, result_null, count, state_ptr);

                } else {
                    ExecuteFlat<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
                }
                // Result tail_index need to update.
                result->Finalize(count);
                return;
            }
            case ColumnVectorType::kConstant: {
                if (count != 1) {
                    String error_message = "Attempting to execute more than one row of the constant column vector.";
                    UnrecoverableError(error_message);
                }
                if (nullable && !(input_null->IsAllTrue())) {
                    result_null->SetFalse(0);
                } else {
                    result_null->SetAllTrue();
                    if constexpr (std::is_same_v<InputType, BooleanT> and std::is_same_v<ResultType, BooleanT>) {
                        BooleanT result_value;
                        Operator::template Execute(input->buffer_->GetCompactBit(0), result_value, result_null.get(), 0, state_ptr);
                        result->buffer_->SetCompactBit(0, result_value);
                    } else if constexpr (std::is_same_v<InputType, EmbeddingT>) {
                        EmbeddingT embedding_input(input->data(), false);
                        Operator::template Execute<InputType, ResultType>(embedding_input, result_ptr[0], result_null.get(), 0, state_ptr);
                        embedding_input.Reset();
                    } else {
                        Operator::template Execute<InputType, ResultType>(input_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
                    }
                }
                result->Finalize(1);
                return;
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
            }
        }
        String error_message = "Unexpected error.";
        UnrecoverableError(error_message);
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
        *result_null = *input_null;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
            if (row_index >= count) {
                return false;
            }
            Operator::template Execute<InputType, ResultType>(input_ptr[row_index], result_ptr[row_index], result_null.get(), row_index, state_ptr);
            return row_index + 1 < count;
        });
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

    template <typename Operator>
    static void inline ExecuteBoolean(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr) {
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;
        result_null->SetAllTrue();
        SizeT count_bytes = count / 8;
        SizeT count_tail = count % 8;
        auto input_u8 = reinterpret_cast<const u8 *>(input->data());
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        for (SizeT i = 0; i < count_bytes; ++i) {
            Operator::template Execute(input_u8[i], result_u8[i], result_null.get(), 0, state_ptr);
        }
        if (count_tail > 0) {
            u8 &tail_u8 = result_u8[count_bytes];
            u8 ans;
            Operator::template Execute(input_u8[count_bytes], ans, result_null.get(), 0, state_ptr);
            u8 keep_mask = u8(0xff) << count_tail;
            tail_u8 = (tail_u8 & keep_mask) | (ans & ~keep_mask);
        }
    }

    template <typename Operator>
    static void inline ExecuteBooleanWithNull(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr) {
        const auto &input_null = input->nulls_ptr_;
        auto &result_null = result->nulls_ptr_;
        *result_null = *input_null;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) -> bool {
            if (row_index >= count) {
                return false;
            }
            // This row isn't null
            BooleanT answer;
            Operator::template Execute(input->buffer_->GetCompactBit(row_index), answer, result_null.get(), row_index, state_ptr);
            result->buffer_->SetCompactBit(row_index, answer);
            return row_index + 1 < count;
        });
    }
};

} // namespace infinity
