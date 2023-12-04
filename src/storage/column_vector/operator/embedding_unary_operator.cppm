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
import bitmask;
import column_vector;
import infinity_exception;

export module embedding_unary_operator;

namespace infinity {

export class EmbeddingUnaryOperator {
public:
    template <typename InputElemType, typename OutputElemType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &input, SharedPtr<ColumnVector> &result, SizeT count, void *state_ptr, bool nullable) {
        const auto *input_ptr = (const InputElemType *)(input->data_ptr_);
        const SharedPtr<Bitmask> &input_null = input->nulls_ptr_;

        auto *result_ptr = (OutputElemType *)(result->data_ptr_);
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        switch (input->vector_type()) {
            case ColumnVectorType::kInvalid: {
                Error<TypeException>("Invalid column vector type.");
            }
            case ColumnVectorType::kConstant: {
                Assert<TypeException>(count == 1, "Attempting to execute more than one row of the constant column vector.");
                if (nullable) {
                    result_null->SetAllTrue();
                    Operator::template Execute<InputElemType, OutputElemType>(input_ptr, result_ptr, result_null.get(), 0, state_ptr);
                } else {
                    result_null->SetFalse(0);
                }
                result->tail_index_ = 1;
                return;
            }
            default: {
                Error<NotImplementException>("Not implement non-constant column vector.");
            }
        }
    }
};

} // namespace infinity