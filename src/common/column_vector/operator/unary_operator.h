//
// Created by JinHai on 2022/11/11.
//

#pragma once

#include "common/column_vector/column_vector.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class UnaryOperator {
public:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline
    Execute(const ColumnVector& input, ColumnVector& result, size_t count, void* state_ptr, bool nullable) {
        const auto* input_ptr = (const InputType*)(input.data_ptr_);
        const SharedPtr<Bitmask>& input_null = input.nulls_ptr_;

        auto* result_ptr = (ResultType*)(result.data_ptr_);
        SharedPtr<Bitmask>& result_null = result.nulls_ptr_;

        switch(input.vector_type()) {
            case ColumnVectorType::kInvalid: {
                TypeError("Invalid column vector type.");
            }
            case ColumnVectorType::kFlat: {
                TypeAssert(result.vector_type() == ColumnVectorType::kFlat, "Target vector type isn't flat.");
                if(nullable) {
                    ExecuteFlatWithNull<InputType, ResultType, Operator>(input_ptr,
                                                                         input_null,
                                                                         result_ptr,
                                                                         result_null,
                                                                         count,
                                                                         state_ptr);
                    // Result tail_index need to update.
                    result.tail_index_ = count;
                    return ;
                } else {
                    return ExecuteFlat<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
                }

            }
            case ColumnVectorType::kConstant: {
                result.SetVectorType(ColumnVectorType::kConstant);
                if(nullable) {
                    if(input.nulls_ptr_->IsAllTrue()) {
                        result_null->SetAllTrue();
                        result_ptr[0] = Operator::template Execute<InputType, ResultType>(input_ptr[0],
                                                                                          result_null.get(),
                                                                                          0,
                                                                                          state_ptr);
                        return ;
                    } else {
                        result_null->SetFalse(0);
                        return ;
                    }
                } else {
                    result_ptr[0] = Operator::template Execute<InputType, ResultType>(input_ptr[0],
                                                                                      result_null.get(),
                                                                                      0,
                                                                                      state_ptr);
                }
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<InputType, ResultType, Operator>(input_ptr, result_ptr, result_null, count, state_ptr);
            }
        }

        GeneralError("Unexpected error.");
    }

private:
    template <typename InputType, typename ResultType, typename Operator>
    static void inline
    ExecuteFlat(const InputType* __restrict input_ptr,
                ResultType* __restrict result_ptr,
                SharedPtr<Bitmask>& result_null,
                size_t count,
                void* state_ptr) {
        for (size_t i = 0; i < count; i++) {
            result_ptr[i] = Operator::template Execute<InputType, ResultType>(input_ptr[i], result_null.get(), i, state_ptr);
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline
    ExecuteFlatWithNull(const InputType* __restrict input_ptr,
                        const SharedPtr<Bitmask>& input_null,
                        ResultType* __restrict result_ptr,
                        SharedPtr<Bitmask>& result_null,
                        size_t count,
                        void* state_ptr) {
        if(input_null->IsAllTrue()) {
            // Initialized all true to output null bitmask.
            result_null->SetAllTrue();

            for (size_t i = 0; i < count; i++) {
                // Not valid for embedding type, since the embedding type width isn't sizeof(EmbeddingT)
                result_ptr[i] = Operator::template Execute<InputType, ResultType>(input_ptr[i], result_null.get(), i, state_ptr);
            }
        } else {
            result_null->DeepCopy(*input_null);

            const u64* input_null_data = input_null->GetData();
            size_t unit_count = BitmaskBuffer::UnitCount(count);
            for(size_t i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++ i, end_index += BitmaskBuffer::UNIT_BITS) {
                if(input_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                    // all data of 64 rows are not null
                    while(start_index < end_index) {
                        result_ptr[i] = Operator::template Execute<InputType, ResultType>(input_ptr[i],
                                                                                          result_null.get(),
                                                                                          start_index ++,
                                                                                          state_ptr);
                    }
                } else if(input_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                    // all data of 64 rows are null
                    ;
                } else {
                    size_t original_start = start_index;
                    while(start_index < end_index) {
                        if(input_null->IsTrue(start_index - original_start)) {
                            // This row isn't null
                            result_ptr[i] = Operator::template Execute<InputType, ResultType>(
                                    input_ptr[i],
                                    result_null.get(),
                                    start_index ++,
                                    state_ptr);
                        }
                    }
                }
            }
        }
    }

    template <typename InputType, typename ResultType, typename Operator>
    static void inline
    ExecuteHeterogeneous(const InputType* __restrict input_ptr,
                         ResultType* __restrict result_ptr,
                         SharedPtr<Bitmask>& result_null,
                         size_t count,
                         void* state_ptr) {
        for (size_t i = 0; i < count; i++) {
            result_ptr[i] = Operator::template Execute<InputType, ResultType>(input_ptr[i], result_null.get(), i, state_ptr);
        }
    }
};

}
