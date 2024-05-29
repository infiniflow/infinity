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
import bitmask;
import bitmask_buffer;
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
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            case ColumnVectorType::kCompactBit: {
                if (result->vector_type() != ColumnVectorType::kCompactBit) {
                    String error_message = "Target vector type isn't kCompactBit.";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
                if constexpr (!std::is_same_v<std::remove_cv_t<InputType>, BooleanT> || !std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    String error_message = "kCompactBit should match with BooleanT.";
                    LOG_CRITICAL(error_message);
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
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
                if constexpr (std::is_same_v<std::remove_cv_t<InputType>, BooleanT> || std::is_same_v<std::remove_cv_t<ResultType>, BooleanT>) {
                    String error_message = "BooleanT type should not be kFlat.";
                    LOG_CRITICAL(error_message);
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
                    LOG_CRITICAL(error_message);
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
        LOG_CRITICAL(error_message);
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
                        Operator::template Execute<InputType, ResultType>(input_ptr[start_index],
                                                                          result_ptr[start_index],
                                                                          result_null.get(),
                                                                          start_index,
                                                                          state_ptr);
                        ++start_index;
                    }
                } else if (input_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    ;
                } else {
                    SizeT original_start = start_index;
                    while (start_index < end_index) {
                        if (input_null->IsTrue(start_index - original_start)) {
                            // This row isn't null
                            Operator::template Execute<InputType, ResultType>(input_ptr[start_index],
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
        const SharedPtr<Bitmask> &input_null = input->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;
        result_null->DeepCopy(*input_null);
        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto input_u8 = reinterpret_cast<const u8 *>(input->data());
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                const SizeT e = end_index / 8, tail = end_index % 8;
                for (SizeT b = start_index / 8; b < e; ++b) {
                    Operator::template Execute(input_u8[b], result_u8[b], result_null.get(), 0, state_ptr);
                }
                if (tail) {
                    u8 tail_result;
                    Operator::template Execute(input_u8[e], tail_result, result_null.get(), 0, state_ptr);
                    const u8 mask_keep = u8(0xff) << tail;
                    result_u8[e] = (result_u8[e] & mask_keep) | (tail_result & ~mask_keep);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (BooleanT answer; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(input->buffer_->GetCompactBit(start_index), answer, result_null.get(), start_index, state_ptr);
                        result->buffer_->SetCompactBit(start_index, answer);
                    }
                }
            }
        }
    }
};

} // namespace infinity
