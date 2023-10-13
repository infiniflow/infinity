//
// Created by JinHai on 2022/9/28.
//

#include "function/scalar/inequals.h"
#include "storage/meta/catalog.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct InEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = (left != right);
    }
};

template <>
inline void InEqualsFunction::Run(VarcharT left, VarcharT right, bool &result) {
    if (left.IsInlined()) {
        if (right.IsInlined()) {
            result = (memcmp(left.prefix, right.prefix, VarcharT::INLINE_LENGTH) != 0);
            return;
        }
    } else if (right.IsInlined()) {
        ;
    } else {
        // Both left and right are not inline
        u16 min_len = std::min(right.length, left.length);
        if (memcmp(left.prefix, right.prefix, VarcharT::PREFIX_LENGTH) != 0) {
            result = true;
        } else {
            result = (memcmp(left.ptr, right.ptr, min_len) != 0);
        }
        return;
    }
    result = false;
}

template <>
inline void InEqualsFunction::Run(MixedT left, BigIntT right, bool &result) {
    NotImplementError("Not implement: mixed == bigint")
}

template <>
inline void InEqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <>
inline void InEqualsFunction::Run(MixedT left, DoubleT right, bool &result) {
    NotImplementError("Not implement: mixed == double")
}

template <>
inline void InEqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <>
inline void InEqualsFunction::Run(MixedT left, VarcharT right, bool &result) {
    NotImplementError("Not implement: mixed == varchar")
}

template <>
inline void InEqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <typename CompareType>
static void GenerateInEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = "<>";
    ScalarFunction inequals_function(func_name,
                                     {data_type, data_type},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(inequals_function);
}

void RegisterInEqualsFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "<>";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateInEqualsFunction<BooleanT>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateInEqualsFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateInEqualsFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateInEqualsFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateInEqualsFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateInEqualsFunction<HugeIntT>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateInEqualsFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateInEqualsFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateInEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateInEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateInEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateInEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateInEqualsFunction<VarcharT>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateInEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    //    GenerateInEqualsFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
    //    GenerateInEqualsFunction<TimeT>(function_set_ptr, DataType(LogicalType::kTime));
    //    GenerateInEqualsFunction<DateTimeT>(function_set_ptr, DataType(LogicalType::kDateTime));
    //    GenerateInEqualsFunction<TimestampT>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateInEqualsFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));
    //    GenerateInEqualsFunction<IntervalT>(function_set_ptr, DataType(LogicalType::kInterval));

    //    GenerateInEqualsFunction<PointT>(function_set_ptr, DataType(LogicalType::kPoint));
    //    GenerateInEqualsFunction<LineT>(function_set_ptr, DataType(LogicalType::kLine));
    //    GenerateInEqualsFunction<LineSegT>(function_set_ptr, DataType(LogicalType::kLineSeg));
    //    GenerateInEqualsFunction<BoxT>(function_set_ptr, DataType(LogicalType::kBox));
    //    GenerateInEqualsFunction<PathT>(function_set_ptr, DataType(LogicalType::kPath));
    //    GenerateInEqualsFunction<PolygonT>(function_set_ptr, DataType(LogicalType::kPolygon));
    //    GenerateInEqualsFunction<CircleT>(function_set_ptr, DataType(LogicalType::kCircle));
    //
    //    GenerateInEqualsFunction<BitmapT>(function_set_ptr, DataType(LogicalType::kBitmap));
    //    GenerateInEqualsFunction<UuidT>(function_set_ptr, DataType(LogicalType::kUuid));
    //    GenerateInEqualsFunction<BlobT>(function_set_ptr, DataType(LogicalType::kBlob));
    //    GenerateInEqualsFunction<EmbeddingT>(function_set_ptr, DataType(LogicalType::kEmbedding));

    //    GenerateInEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_inequals_bigint(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_bigint);

    ScalarFunction bigint_inequals_mixed(func_name,
                                         {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(bigint_inequals_mixed);

    ScalarFunction mix_inequals_double(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_double);

    ScalarFunction double_inequals_mixed(func_name,
                                         {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(double_inequals_mixed);

    ScalarFunction mix_inequals_varchar(func_name,
                                        {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                        DataType(kBoolean),
                                        &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_varchar);

    ScalarFunction varchar_inequals_mixed(func_name,
                                          {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                          DataType(kBoolean),
                                          &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(varchar_inequals_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
