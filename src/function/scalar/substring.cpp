//
// Created by jinhai on 23-3-20.
//

#include "substring.h"

#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct SubstrFunction {
    template<typename TA, typename TB, typename TC, typename TD>
    static inline bool
    Run(TA first, TB second, TC third, TD& result, ColumnVector* column_vector_ptr) {
        NotImplementError("Not implemented");
    }
};

template<>
inline bool
SubstrFunction::Run(VarcharT first, BigIntT second, BigIntT third, VarcharT& result, ColumnVector* column_vector_ptr) {
    // Validate the input before slice the string
    if(second < 0) {
        ExecutorError(fmt::format("substring start offset should >= 0, currently it is {}", second));
    }

    if(third < 0) {
        ExecutorError(fmt::format("substring length should >= 0, currently it is {}", third));
    }

    if(third == 0) {
        // Construct empty varchar value;
        result.InitializeAsEmptyStr();
        return true;
    }

    SizeT source_len = first.GetDataLen();
    if(second >= source_len) {
        // Construct empty varchar value;
        result.InitializeAsEmptyStr();
        return true;
    }

    SizeT start_offset = second;
    SizeT end_offset = 0;
    if(start_offset + third >= source_len) {
        end_offset = source_len;
    } else {
        end_offset = start_offset + third;
    }

    SizeT copied_length = end_offset - start_offset;
    ptr_t source_ptr = first.GetDataPtr();
    if(copied_length <= VarcharT::INLINE_LENGTH) {
        // inline varchar
        memcpy(result.prefix, source_ptr + start_offset, copied_length);
        result.length = copied_length;
    } else {
        memcpy(result.prefix, source_ptr + start_offset, VarcharT::INLINE_LENGTH);
        result.ptr = column_vector_ptr->buffer_->heap_mgr_->Allocate(copied_length);
        memcpy(result.ptr, source_ptr + start_offset, copied_length);
    }

    return true;
}

void
RegisterSubstringFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "substring";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_substr(
            func_name,
            { DataType(LogicalType::kVarchar), DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kVarchar) },
            &ScalarFunction::TernaryFunctionToVarlenWithFailure<VarcharT, BigIntT, BigIntT, VarcharT, SubstrFunction>);
    function_set_ptr->AddFunction(varchar_substr);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}