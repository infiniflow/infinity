//
// Created by jinhai on 23-3-19.
//

#include "extract.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct ExtractYearFunction {
    template<typename TA, typename TB>
    static inline void
    Run(TA input, TB& result) {
        NotImplementError("ExtractYear function isn't implemented")
    }
};

template<>
inline void
ExtractYearFunction::Run(DateT left, BigIntT& result) {
    result = DateT::GetDatePart(left, TimeUnit::kYear);
}

struct ExtractMonthFunction {
    template<typename TA, typename TB>
    static inline void
    Run(TA input, TB& result) {
        NotImplementError("ExtractMonth function isn't implemented")
    }
};

template<>
inline void
ExtractMonthFunction::Run(DateT left, BigIntT& result) {
    result = DateT::GetDatePart(left, TimeUnit::kMonth);
}


struct ExtractDayFunction {
    template<typename TA, typename TB>
    static inline void
    Run(TA input, TB& result) {
        NotImplementError("ExtractMonth function isn't implemented")
    }
};

template<>
inline void
ExtractDayFunction::Run(DateT left, BigIntT& result) {
    result = DateT::GetDatePart(left, TimeUnit::kDay);
}

void
RegisterExtractFunction(const UniquePtr<NewCatalog>& catalog_ptr) {
    {
        String func_name = "extract_year";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_year_from_date(
                func_name,
                {DataType(LogicalType::kDate)},
                DataType(kBigInt),
                &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractYearFunction>);
        function_set_ptr->AddFunction(extract_year_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_month";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_month_from_date(
                func_name,
                {DataType(LogicalType::kDate)},
                DataType(kBigInt),
                &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractMonthFunction>);
        function_set_ptr->AddFunction(extract_month_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }

    {
        String func_name = "extract_day";
        SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
        ScalarFunction extract_day_from_date(
                func_name,
                {DataType(LogicalType::kDate)},
                DataType(kBigInt),
                &ScalarFunction::UnaryFunction<DateT, BigIntT, ExtractDayFunction>);
        function_set_ptr->AddFunction(extract_day_from_date);
        NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
    }
}

}
