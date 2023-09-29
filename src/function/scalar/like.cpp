//
// Created by jinhai on 23-3-18.
//

#include "like.h"
#include "less_equals.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

bool
LikeOperator(const ptr_t left_ptr, SizeT left_len, const ptr_t right_ptr, SizeT right_len) {
    SizeT left_idx{0}, right_idx{0};

    while(right_idx < right_len) {
        char left_char = left_ptr[left_idx];
        char right_char = right_ptr[right_idx];
        if(right_char == '_' or (left_char == right_char)) {
            ++left_idx;
            ++right_idx;
        } else if(right_char == '%') {
            ++right_idx;

            // If there are more than one %
            while(right_idx < right_len && right_ptr[right_idx] == '%') {
                ++right_idx;
            }

            // % is the last character
            if(right_idx == right_len) {
                return true;
            }

            // Check any left part is matched with rest of right part.
            while(left_idx < left_len) {
                if(LikeOperator(&left_ptr[left_idx],
                                left_len - left_idx,
                                &right_ptr[right_idx],
                                right_len - right_idx)) {
                    return true;
                }
                ++left_idx;
            }

            // Not matched
            return false;
        } else {
            return false;
        }
    }

    while(right_idx < right_len && right_ptr[right_idx] == '%') {
        ++right_idx;
    }

    return left_idx == left_len && right_idx == right_len;
}

struct LikeFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        NotImplementError("Not implement like function")
    }
};

template<>
inline void
LikeFunction::Run(VarcharT& left, VarcharT& right, bool& result) {
    ptr_t left_ptr = left.GetDataPtr();
    SizeT left_len = left.GetDataLen();
    ptr_t right_ptr = right.GetDataPtr();
    SizeT right_len = right.GetDataLen();

    result = LikeOperator(left_ptr, left_len, right_ptr, right_len);
}

struct NotLikeFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        NotImplementError("Not implement not like function")
    }
};

template<>
inline void
NotLikeFunction::Run(VarcharT& left, VarcharT& right, bool& result) {
    ptr_t left_ptr = left.GetDataPtr();
    SizeT left_len = left.GetDataLen();
    ptr_t right_ptr = right.GetDataPtr();
    SizeT right_len = right.GetDataLen();

    result = !LikeOperator(left_ptr, left_len, right_ptr, right_len);
}

void
RegisterLikeFunction(const UniquePtr<NewCatalog>& catalog_ptr) {
    String func_name = "like";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_like_function(
            func_name,
            {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
            DataType(kBoolean),
            &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, LikeFunction>);
    function_set_ptr->AddFunction(varchar_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

void
RegisterNotLikeFunction(const UniquePtr<NewCatalog>& catalog_ptr) {
    String func_name = "not_like";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_not_like_function(
            func_name,
            {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
            DataType(kBoolean),
            &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, NotLikeFunction>);
    function_set_ptr->AddFunction(varchar_not_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}