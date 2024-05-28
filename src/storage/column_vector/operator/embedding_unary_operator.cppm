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

export module embedding_unary_operator;

import stl;
import bitmask;
import column_vector;
import infinity_exception;

import bitmask_buffer;
import embedding_info;
import logger;

namespace infinity {

export class EmbeddingUnaryOperator {
public:
    template <typename InputElemType, typename OutputElemType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr, bool nullable) {
        const auto *input_ptr = (const InputElemType *)(input->data());
        const SharedPtr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (OutputElemType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        auto embedding_info = static_cast<EmbeddingInfo *>(input->data_type()->type_info().get());
        SizeT dim = embedding_info->Dimension();

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                String error_message = "Invalid column vector type.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                break;
            }
            case ColumnVectorType::kFlat: {
                if (result->vector_type() != ColumnVectorType::kFlat) {
                    String error_message = "Target vector type isn't flat.";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                    break;
                }
                if (nullable) {
                    ExecuteFlatWithNull<InputElemType, OutputElemType, Operator>(input_ptr,
                                                                                 input_null,
                                                                                 result_ptr,
                                                                                 result_null,
                                                                                 dim,
                                                                                 count,
                                                                                 state_ptr);
                } else {
                    ExecuteFlat<InputElemType, OutputElemType, Operator>(input_ptr, result_ptr, dim, result_null.get(), count, state_ptr);
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
                if (nullable) {
                    result_null->SetAllTrue();
                    Operator::template Execute<InputElemType, OutputElemType>(input_ptr, result_ptr, dim, result_null.get(), 0, state_ptr);
                } else {
                    result_null->SetFalse(0);
                }
                result->Finalize(1);
                return;
            }
            case ColumnVectorType::kHeterogeneous: {
                String error_message = "Heterogeneous embedding is not implemented yet.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                // return ExecuteHeterogeneous<InputElemType, OutputElemType, Operator>(input, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kCompactBit: {
                String error_message = "Compact Bit embedding is not implemented yet.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
                // return ExecuteHeterogeneous<InputElemType, OutputElemType, Operator>(input, result, count, state_ptr, nullable);
            }
        }
    }

private:
    template <typename InputElemType, typename OutputElemType, typename Operator>
    static void inline ExecuteFlat(const InputElemType *input_ptr,
                                   OutputElemType *result_ptr,
                                   SizeT dim,
                                   Bitmask *result_null,
                                   SizeT count,
                                   void *state_ptr) {
        for (SizeT i = 0; i < count; ++i) {
            Operator::template Execute<InputElemType, OutputElemType>(input_ptr + dim * i, result_ptr + dim * i, dim, result_null, i, state_ptr);
        }
    }

    template <typename InputElemType, typename ResultElemType, typename Operator>
    static void inline ExecuteFlatWithNull(const InputElemType *input_ptr,
                                           const SharedPtr<Bitmask> &input_null,
                                           ResultElemType *result_ptr,
                                           SharedPtr<Bitmask> &result_null,
                                           SizeT dim,
                                           SizeT count,
                                           void *state_ptr) {
        if (input_null->IsAllTrue()) {
            // Initialized all true to output null bitmask.
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; ++i) {
                Operator::template Execute<InputElemType, ResultElemType>(input_ptr + dim * i,
                                                                          result_ptr + dim * i,
                                                                          dim,
                                                                          result_null.get(),
                                                                          i,
                                                                          state_ptr);
            }
        } else {
            result_null->DeepCopy(*input_null);

            const u64 *input_null_data = input_null->GetData();
            SizeT unit_count = BitmaskBuffer::UnitCount(count);
            for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
                if (input_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while (start_index < end_index) {
                        Operator::template Execute<InputElemType, ResultElemType>(input_ptr + dim * start_index,
                                                                                  result_ptr + dim * start_index,
                                                                                  dim,
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
                            Operator::template Execute<InputElemType, ResultElemType>(input_ptr + dim * start_index,
                                                                                      result_ptr + dim * start_index,
                                                                                      dim,
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