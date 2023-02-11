//
// Created by jinhai on 23-1-7.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/scalar/plus.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class PlusFunctionsTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(PlusFunctionsTest, plus_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterPlusFunction(catalog_ptr);

    String op = "+";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kTinyInt);
        DataType result_type(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                1,
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(TinyInt)->TinyInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v1.value_.tiny_int, static_cast<i8>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kSmallInt);
        DataType result_type(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(SmallInt)->SmallInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v1.value_.small_int, static_cast<i16>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v.value_.small_int, static_cast<i16>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kInteger);
        DataType result_type(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Integer)->Integer", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v1.value_.integer, static_cast<i32>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v.value_.integer, static_cast<i32>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kBigInt);
        DataType result_type(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(BigInt)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v1.value_.big_int, static_cast<i64>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, static_cast<i64>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kHugeInt);
        DataType result_type(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(HugeInt)->HugeInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeHugeInt(HugeIntT(static_cast<i64>(i), static_cast<i64>(i))));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(v1.value_.huge_int.lower, static_cast<i64>(i));
            EXPECT_EQ(v1.value_.huge_int.upper, static_cast<i64>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(v.value_.huge_int.lower, static_cast<i64>(i));
            EXPECT_EQ(v.value_.huge_int.upper, static_cast<i64>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kFloat);
        DataType result_type(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Float)->Float", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<f32>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(v1.value_.float32, static_cast<f32>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(v.value_.float32, static_cast<f32>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDouble);
        DataType result_type(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Double)->Double", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<f64>(i)));
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(v1.value_.float64, static_cast<f64>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(v.value_.float64, static_cast<f64>(i));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDecimal16);
        DataType result_type(LogicalType::kDecimal16);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Decimal16)->Decimal16", func.ToString().c_str());

        // TODO: complete it.
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDecimal32);
        DataType result_type(LogicalType::kDecimal32);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Decimal32)->Decimal32", func.ToString().c_str());

        // TODO: complete it.
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDecimal64);
        DataType result_type(LogicalType::kDecimal64);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Decimal64)->Decimal64", func.ToString().c_str());

        // TODO: complete it.
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDecimal128);
        DataType result_type(LogicalType::kDecimal128);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Decimal128)->Decimal128", func.ToString().c_str());

        // TODO: complete it.
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kMixed);
        DataType result_type(LogicalType::kMixed);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 1,
                                                                                 "c1",
                                                                                 0,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("+(Heterogeneous)->Heterogeneous", func.ToString().c_str());

        // TODO: complete it.
    }
}