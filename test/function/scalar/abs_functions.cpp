//
// Created by jinhai on 23-1-5.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/scalar/abs.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class AbsFunctionsTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(AbsFunctionsTest, abs_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterAbsFunction(catalog_ptr);

    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName("abs");
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(TinyInt)->TinyInt", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(i));
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            i8 res = static_cast<i8>(i);
            if (res == std::numeric_limits<i8>::min()) {
                EXPECT_EQ(result.nulls_ptr_->IsTrue(i), false);
            } else {
                EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
                EXPECT_EQ(v.value_.tiny_int, abs(res));
            }
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(SmallInt)->SmallInt", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<i16>(-i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(v.value_.small_int, -static_cast<i16>(i));
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kSmallInt);
            i16 res = static_cast<i16>(i);
            EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
            EXPECT_EQ(v.value_.small_int, abs(res));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(Integer)->Integer", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<i32>(-i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(v.value_.integer, -static_cast<i32>(i));
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kInteger);
            i32 res = static_cast<i32>(i);
            EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
            EXPECT_EQ(v.value_.integer, abs(res));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(BigInt)->BigInt", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<i64>(-i)));
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(v.value_.big_int, -static_cast<i64>(i));
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBigInt);
            i64 res = static_cast<i64>(i);
            EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
            EXPECT_EQ(v.value_.big_int, abs(res));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(Float)->Float", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            f32 input = i - 16384;
            input -= 0.5;
            data_block.AppendValue(0, Value::MakeFloat(input));
        }
        data_block.Finalize();

        for (i64 i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kFloat);

            f32 input = i - 16384;
            input -= 0.5;
            EXPECT_FLOAT_EQ(v.value_.float32, input);
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (i64 i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kFloat);
            f32 input = i - 16384;
            input -= 0.5;
            EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
            EXPECT_FLOAT_EQ(v.value_.float32, fabs(input));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("abs(Double)->Double", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);

        i64 row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (i64 i = 0; i < row_count; ++i) {
            f64 input = i - 16384;
            input -= 0.5;
            data_block.AppendValue(0, Value::MakeDouble(input));
        }
        data_block.Finalize();

        for (i64 i = 0; i < row_count; ++i) {
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDouble);

            f64 input = i - 16384;
            input -= 0.5;
            EXPECT_FLOAT_EQ(v.value_.float64, input);
        }

        ColumnVector result(data_type);
        result.Initialize();
        func.function_(data_block, result);

        for (i64 i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kDouble);
            f64 input = i - 16384;
            input -= 0.5;
            EXPECT_EQ(result.nulls_ptr_->IsTrue(i), true);
            EXPECT_FLOAT_EQ(v.value_.float64, fabs(input));
        }
    }

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        inputs.emplace_back(col_expr_ptr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);

        EXPECT_STREQ("abs(Float)->Float", func.ToString().c_str());
    }
}