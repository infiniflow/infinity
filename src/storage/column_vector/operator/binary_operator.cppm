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
import column_vector;

import infinity_exception;
import bitmask;
import bitmask_buffer;

export module binary_operator;

namespace infinity {

export class BinaryOperator {
public:
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {

        switch (left->vector_type()) {
            case ColumnVectorType::kInvalid: {
                Error<TypeException>("Invalid column vector type.");
            }
            case ColumnVectorType::kFlat: {
                return ExecuteFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
        }
    }

private:
    // Level-1 switch case functions
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlat(const SharedPtr<ColumnVector> &left,
                                   const SharedPtr<ColumnVector> &right,
                                   SharedPtr<ColumnVector> &result,
                                   SizeT count,
                                   void *state_ptr,
                                   bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                Error<TypeException>("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteFlatFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteFlatConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteFlatHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstant(const SharedPtr<ColumnVector> &left,
                                       const SharedPtr<ColumnVector> &right,
                                       SharedPtr<ColumnVector> &result,
                                       SizeT count,
                                       void *state_ptr,
                                       bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                Error<TypeException>("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteConstantFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstantConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteConstantHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const SharedPtr<ColumnVector> &left,
                                            const SharedPtr<ColumnVector> &right,
                                            SharedPtr<ColumnVector> &result,
                                            SizeT count,
                                            void *state_ptr,
                                            bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                Error<TypeException>("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteHeterogeneousFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteHeterogeneousConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneousHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
        }
    }

private:
    // Level-2 switch case function
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlat(const SharedPtr<ColumnVector> &left,
                                       const SharedPtr<ColumnVector> &right,
                                       SharedPtr<ColumnVector> &result,
                                       SizeT count,
                                       void *state_ptr,
                                       bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteFlatFlatWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                               left_null,
                                                                               right_ptr,
                                                                               right_null,
                                                                               result_ptr,
                                                                               result_null,
                                                                               count,
                                                                               state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                            right_ptr[i],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlatWithNull(const LeftType *__restrict left_ptr,
                                               const SharedPtr<Bitmask> &left_null,
                                               const RightType *__restrict right_ptr,
                                               const SharedPtr<Bitmask> &right_null,
                                               ResultType *__restrict result_ptr,
                                               SharedPtr<Bitmask> &result_null,
                                               SizeT count,
                                               void *state_ptr) {

        if (left_null->IsAllTrue()) {
            if (right_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                                right_ptr[i],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                result_null->DeepCopy(*right_null);
            }
        } else {
            if (right_null->IsAllTrue()) {
                result_null->DeepCopy(*left_null);
            } else {
                result_null->DeepCopy(*right_null);
                result_null->Merge(*left_null);
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                right_ptr[start_index],
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
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                    right_ptr[start_index],
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

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstant(const SharedPtr<ColumnVector> &left,
                                           const SharedPtr<ColumnVector> &right,
                                           SharedPtr<ColumnVector> &result,
                                           SizeT count,
                                           void *state_ptr,
                                           bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteFlatConstantWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                                   left_null,
                                                                                   right_ptr,
                                                                                   right_null,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                            right_ptr[0],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstantWithNull(const LeftType *__restrict left_ptr,
                                                   const SharedPtr<Bitmask> &left_null,
                                                   const RightType *__restrict right_ptr,
                                                   const SharedPtr<Bitmask> &right_null,
                                                   ResultType *__restrict result_ptr,
                                                   SharedPtr<Bitmask> &result_null,
                                                   SizeT count,
                                                   void *state_ptr) {

        if (left_null->IsAllTrue()) {
            if (right_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                                right_ptr[0],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        } else {
            if (right_null->IsAllTrue()) {
                result_null->DeepCopy(*left_null);
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                right_ptr[0],
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
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                    right_ptr[0],
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

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatHeterogeneous(const SharedPtr<ColumnVector> &,
                                                const SharedPtr<ColumnVector> &,
                                                SharedPtr<ColumnVector> &,
                                                SizeT ,
                                                void *,
                                                bool ) {
        Error<TypeException>("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlat(const SharedPtr<ColumnVector> &left,
                                           const SharedPtr<ColumnVector> &right,
                                           SharedPtr<ColumnVector> &result,
                                           SizeT count,
                                           void *state_ptr,
                                           bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteConstantFlatWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                                   left_null,
                                                                                   right_ptr,
                                                                                   right_null,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                            right_ptr[i],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlatWithNull(const LeftType *__restrict left_ptr,
                                                   const SharedPtr<Bitmask> &left_null,
                                                   const RightType *__restrict right_ptr,
                                                   const SharedPtr<Bitmask> &right_null,
                                                   ResultType *__restrict result_ptr,
                                                   SharedPtr<Bitmask> &result_null,
                                                   SizeT count,
                                                   void *state_ptr) {

        if (right_null->IsAllTrue()) {
            if (left_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                right_ptr[i],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        } else {
            if (left_null->IsAllTrue()) {
                result_null->DeepCopy(*right_null);
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                right_ptr[start_index],
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
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                    right_ptr[start_index],
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

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantConstant(const SharedPtr<ColumnVector> &left,
                                               const SharedPtr<ColumnVector> &right,
                                               SharedPtr<ColumnVector> &result,
                                               SizeT,
                                               void *state_ptr,
                                               bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            if (left_null->IsAllTrue() && right_null->IsAllTrue()) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                            right_ptr[0],
                                                                            result_ptr[0],
                                                                            result_null.get(),
                                                                            0,
                                                                            state_ptr);
            } else {
                result_null->SetAllFalse();
            }
        } else {
            result_null->SetAllTrue();
            Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0], right_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
        }
        result->Finalize(1) ;
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantHeterogeneous(const SharedPtr<ColumnVector> &,
                                                    const SharedPtr<ColumnVector> &,
                                                    SharedPtr<ColumnVector> &,
                                                    SizeT ,
                                                    void *,
                                                    bool ) {
        Error<TypeException>("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousFlat(const SharedPtr<ColumnVector> &,
                                                const SharedPtr<ColumnVector> &,
                                                SharedPtr<ColumnVector> &,
                                                SizeT ,
                                                void *,
                                                bool ) {
        Error<TypeException>("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousConstant(const SharedPtr<ColumnVector> &,
                                                    const SharedPtr<ColumnVector> &,
                                                    SharedPtr<ColumnVector> &,
                                                    SizeT ,
                                                    void *,
                                                    bool ) {
        Error<TypeException>("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousHeterogeneous(const SharedPtr<ColumnVector> &,
                                                         const SharedPtr<ColumnVector> &,
                                                         SharedPtr<ColumnVector> &,
                                                         SizeT ,
                                                         void *,
                                                         bool ) {
        Error<TypeException>("Not implemented.");
    }
};

} // namespace infinity
