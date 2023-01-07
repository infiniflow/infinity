//
// Created by JinHai on 2022/11/11.
//

#pragma once

#include "common/column_vector/column_vector.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class BinaryOperator {
public:
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline Execute(const ColumnVector& left,
                               const ColumnVector& right,
                               ColumnVector& result,
                               size_t count,
                               void* state_ptr,
                               bool nullable) {
        switch(left.vector_type()) {
            case ColumnVectorType::kInvalid: {
                GeneralError("Invalid column vector type.");
            }
            case ColumnVectorType::kFlat: {
                return ExecuteFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                  right,
                                                                                  result,
                                                                                  count,
                                                                                  state_ptr,
                                                                                  nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                       right,
                                                                                       result,
                                                                                       count,
                                                                                       state_ptr,
                                                                                       nullable);
            }
        }
    }

private:

    // Level-1 switch case functions
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlat(const ColumnVector& left,
                                   const ColumnVector& right,
                                   ColumnVector& result,
                                   size_t count,
                                   void* state_ptr,
                                   bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteFlatFlat<LeftType, RightType, ResultType, Operator>(left,
                                                                                  right,
                                                                                  result,
                                                                                  count,
                                                                                  state_ptr,
                                                                                  nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteFlatConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                      right,
                                                                                      result,
                                                                                      count,
                                                                                      state_ptr,
                                                                                      nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteFlatHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                           right,
                                                                                           result,
                                                                                           count,
                                                                                           state_ptr,
                                                                                           nullable);
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstant(const ColumnVector& left,
                                       const ColumnVector& right,
                                       ColumnVector& result,
                                       size_t count,
                                       void* state_ptr,
                                       bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteConstantFlat<LeftType, RightType, ResultType, Operator>(left,
                                                                                      right,
                                                                                      result,
                                                                                      count,
                                                                                      state_ptr,
                                                                                      nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstantConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                          right,
                                                                                          result,
                                                                                          count,
                                                                                          state_ptr,
                                                                                          nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteConstantHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                               right,
                                                                                               result,
                                                                                               count,
                                                                                               state_ptr,
                                                                                               nullable);
            }
        }

    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const ColumnVector& left,
                                            const ColumnVector& right,
                                            ColumnVector& result,
                                            size_t count,
                                            void* state_ptr,
                                            bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteHeterogeneousFlat<LeftType, RightType, ResultType, Operator>(left,
                                                                                           right,
                                                                                           result,
                                                                                           count,
                                                                                           state_ptr,
                                                                                           nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteHeterogeneousConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                               right,
                                                                                               result,
                                                                                               count,
                                                                                               state_ptr,
                                                                                               nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneousHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                                    right,
                                                                                                    result,
                                                                                                    count,
                                                                                                    state_ptr,
                                                                                                    nullable);
            }
        }
    }

private:
    // Level-2 switch case function
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlat(const ColumnVector& left,
                                   const ColumnVector& right,
                                   ColumnVector& result,
                                   size_t count,
                                   void* state_ptr,
                                   bool nullable) {
        const auto* left_ptr = (const LeftType*)(left.data_ptr_);
        const auto* right_ptr = (const RightType*)(right.data_ptr_);
        auto* result_ptr = (ResultType*)(result.data_ptr_);
        SharedPtr<Bitmask>& result_null = result.nulls_ptr_;

        if(nullable) {
            const SharedPtr<Bitmask>& left_null = left.nulls_ptr_;
            const SharedPtr<Bitmask>& right_null = right.nulls_ptr_;

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

            for (size_t i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(
                        left_ptr[i],
                        right_ptr[i],
                        result_ptr[i],
                        result_null.get(),
                        i,
                        state_ptr);
            }
        }
        result.tail_index_ = count;
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlatWithNull(const LeftType* __restrict left_ptr,
                                       const SharedPtr<Bitmask>& left_null,
                                       const RightType* __restrict right_ptr,
                                       const SharedPtr<Bitmask>& right_null,
                                       ResultType* __restrict result_ptr,
                                       SharedPtr<Bitmask>& result_null,
                                       size_t count,
                                       void* state_ptr) {
        if(left_null->IsAllTrue()) {
            if(right_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (size_t i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(
                            left_ptr[i],
                            right_ptr[i],
                            result_ptr[i],
                            result_null.get(),
                            i,
                            state_ptr);
                }

                return ;
            } else {
                result_null->DeepCopy(*right_null);
            }
        } else {
            if(right_null->IsAllTrue()) {
                result_null->DeepCopy(*left_null);
            } else {
                result_null->DeepCopy(*right_null);
                result_null->Merge(*left_null);
            }
        }

        const u64* result_null_data = result_null->GetData();
        size_t unit_count = BitmaskBuffer::UnitCount(count);
        for(size_t i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++ i, end_index += BitmaskBuffer::UNIT_BITS) {
            if(result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while(start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(
                            left_ptr[i],
                            right_ptr[i],
                            result_ptr[i],
                            result_null.get(),
                            start_index ++,
                            state_ptr);
                }
            } else if(result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                ;
            } else {
                size_t original_start = start_index;
                while(start_index < end_index) {
                    if(result_null->IsTrue(start_index - original_start)) {
                        // This row isn't null
                        Operator::template Execute<LeftType, RightType, ResultType>(
                                left_ptr[i],
                                right_ptr[i],
                                result_ptr[i],
                                result_null.get(),
                                start_index ++,
                                state_ptr);
                    }
                }
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstant(const ColumnVector& left,
                                           const ColumnVector& right,
                                           ColumnVector& result,
                                           size_t count,
                                           void* state_ptr,
                                           bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatHeterogeneous(const ColumnVector& left,
                                                const ColumnVector& right,
                                                ColumnVector& result,
                                                size_t count,
                                                void* state_ptr,
                                                bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlat(const ColumnVector& left,
                                           const ColumnVector& right,
                                           ColumnVector& result,
                                           size_t count,
                                           void* state_ptr,
                                           bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantConstant(const ColumnVector& left,
                                               const ColumnVector& right,
                                               ColumnVector& result,
                                               size_t count,
                                               void* state_ptr,
                                               bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantHeterogeneous(const ColumnVector& left,
                                                    const ColumnVector& right,
                                                    ColumnVector& result,
                                                    size_t count,
                                                    void* state_ptr,
                                                    bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousFlat(const ColumnVector& left,
                                                const ColumnVector& right,
                                                ColumnVector& result,
                                                size_t count,
                                                void* state_ptr,
                                                bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousConstant(const ColumnVector& left,
                                                    const ColumnVector& right,
                                                    ColumnVector& result,
                                                    size_t count,
                                                    void* state_ptr,
                                                    bool nullable) {
        GeneralError("Not implemented.")
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousHeterogeneous(const ColumnVector& left,
                                                         const ColumnVector& right,
                                                         ColumnVector& result,
                                                         size_t count,
                                                         void* state_ptr,
                                                         bool nullable) {
        GeneralError("Not implemented.")
    }
};

}
