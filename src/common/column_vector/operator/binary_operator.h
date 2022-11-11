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
                               ColumnVector& result, size_t count, bool nullable) {
        switch(left.vector_type()) {
            case ColumnVectorType::kInvalid: {
                GeneralError("Invalid column vector type.");
            }
            case ColumnVectorType::kFlat: {
                return ExecuteFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                  right,
                                                                                  result,
                                                                                  count,
                                                                                  nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                       right,
                                                                                       result,
                                                                                       count,
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
                                   bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteFlatFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteFlatConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                      right,
                                                                                      result,
                                                                                      count,
                                                                                      nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteFlatHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                           right,
                                                                                           result,
                                                                                           count,
                                                                                           nullable);
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstant(const ColumnVector& left,
                                       const ColumnVector& right,
                                       ColumnVector& result, size_t count, bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteConstantFlat<LeftType, RightType, ResultType, Operator>(left,
                                                                                      right,
                                                                                      result,
                                                                                      count,
                                                                                      nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstantConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                          right,
                                                                                          result,
                                                                                          count,
                                                                                          nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteConstantHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                               right,
                                                                                               result,
                                                                                               count,
                                                                                               nullable);
            }
        }

    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const ColumnVector& left,
                                            const ColumnVector& right,
                                            ColumnVector& result, size_t count, bool nullable) {
        switch(right.vector_type()) {
            case ColumnVectorType::kInvalid:
                GeneralError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteHeterogeneousFlat<LeftType, RightType, ResultType, Operator>(left,
                                                                                           right,
                                                                                           result,
                                                                                           count,
                                                                                           nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteHeterogeneousConstant<LeftType, RightType, ResultType, Operator>(left,
                                                                                               right,
                                                                                               result,
                                                                                               count,
                                                                                               nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneousHeterogeneous<LeftType, RightType, ResultType, Operator>(left,
                                                                                                    right,
                                                                                                    result,
                                                                                                    count,
                                                                                                    nullable);
            }
        }
    }

private:
    // Level-2 switch case function
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlat(const ColumnVector& left,
                                   const ColumnVector& right,
                                   ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstant(const ColumnVector& left,
                                           const ColumnVector& right,
                                           ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatHeterogeneous(const ColumnVector& left,
                                                const ColumnVector& right,
                                                ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlat(const ColumnVector& left,
                                           const ColumnVector& right,
                                           ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantConstant(const ColumnVector& left,
                                               const ColumnVector& right,
                                               ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantHeterogeneous(const ColumnVector& left,
                                                    const ColumnVector& right,
                                                    ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousFlat(const ColumnVector& left,
                                                const ColumnVector& right,
                                                ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousConstant(const ColumnVector& left,
                                                    const ColumnVector& right,
                                                    ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousHeterogeneous(const ColumnVector& left,
                                                         const ColumnVector& right,
                                                         ColumnVector& result, size_t count, bool nullable) {
        GeneralError("Not implemented.");
    }
}
}
