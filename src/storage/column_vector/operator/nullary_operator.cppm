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

#include <concepts>
#include <type_traits>

export module nullary_operator;

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
    template <typename ResultType, typename Operator>
    static void inline Execute(SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;
        ExecuteFlat<typename ResultType, typename Operator>(ResultType *__restrict result_ptr, int &result_null, int count, void *state_ptr)
        result->Finalize(count);
        return;
        String error_message = "Unexpected error.";
        UnrecoverableError(error_message);
    }

private:
    template <typename ResultType, typename Operator>
    static void inline ExecuteFlat(
                                   ResultType *__restrict result_ptr,
                                   SharedPtr<Bitmask> &result_null,
                                   SizeT count,
                                   void *state_ptr) {
        for (SizeT i = 0; i < count; i++) {
            Operator::template Execute<ResultType>(result_ptr[i], result_null.get(), i, state_ptr);
        }
    }
};

} // namespace infinity
