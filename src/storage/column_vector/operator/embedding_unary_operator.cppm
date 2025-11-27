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

export module infinity_core:embedding_unary_operator;

import :roaring_bitmap;
import :column_vector;
import :infinity_exception;
import :logger;

import embedding_info;

namespace infinity {

export class EmbeddingUnaryOperator {
public:
    template <typename InputElemType, typename OutputElemType, typename Operator>
    static void inline Execute(const std::shared_ptr<ColumnVector> &input,
                               std::shared_ptr<ColumnVector> &result,
                               size_t count,
                               void *state_ptr,
                               bool nullable) {
        const auto *input_ptr = (const InputElemType *)(input->data().get());
        const std::shared_ptr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (OutputElemType *)(result->data().get());
        std::shared_ptr<Bitmask> &result_null = result->nulls_ptr_;

        auto embedding_info = static_cast<EmbeddingInfo *>(input->data_type()->type_info().get());
        size_t dim = embedding_info->Dimension();

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                UnrecoverableError("Invalid column vector type.");
                break;
            }
            case ColumnVectorType::kFlat: {
                if (result->vector_type() != ColumnVectorType::kFlat) {
                    UnrecoverableError("Target vector type isn't flat.");
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
                    UnrecoverableError("Attempting to execute more than one row of the constant column vector.");
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
                UnrecoverableError("Heterogeneous embedding is not implemented yet.");
                // return ExecuteHeterogeneous<InputElemType, OutputElemType, Operator>(input, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kCompactBit: {
                UnrecoverableError("Compact Bit embedding is not implemented yet.");
                // return ExecuteHeterogeneous<InputElemType, OutputElemType, Operator>(input, result, count, state_ptr, nullable);
            }
        }
    }

private:
    template <typename InputElemType, typename OutputElemType, typename Operator>
    static void inline ExecuteFlat(const InputElemType *input_ptr,
                                   OutputElemType *result_ptr,
                                   size_t dim,
                                   Bitmask *result_null,
                                   size_t count,
                                   void *state_ptr) {
        for (size_t i = 0; i < count; ++i) {
            Operator::template Execute<InputElemType, OutputElemType>(input_ptr + dim * i, result_ptr + dim * i, dim, result_null, i, state_ptr);
        }
    }

    template <typename InputElemType, typename ResultElemType, typename Operator>
    static void inline ExecuteFlatWithNull(const InputElemType *input_ptr,
                                           const std::shared_ptr<Bitmask> &input_null,
                                           ResultElemType *result_ptr,
                                           std::shared_ptr<Bitmask> &result_null,
                                           size_t dim,
                                           size_t count,
                                           void *state_ptr) {
        *result_null = *input_null;
        result_null->RoaringBitmapApplyFunc([&](u32 row_index) {
            if (row_index >= count) {
                return false;
            }
            Operator::template Execute<InputElemType, ResultElemType>(input_ptr + dim * row_index,
                                                                      result_ptr + dim * row_index,
                                                                      dim,
                                                                      result_null.get(),
                                                                      row_index,
                                                                      state_ptr);
            return row_index + 1 < count;
        });
    }
};

} // namespace infinity