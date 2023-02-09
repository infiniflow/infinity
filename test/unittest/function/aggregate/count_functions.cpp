//
// Created by jinhai on 23-1-5.
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
#include "function/aggregate/count.h"
#include "function/aggregate_function_set.h"
#include "expression/column_expression.h"

class CountFunctionTest : public BaseTest {
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

TEST_F(CountFunctionTest, count_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterCountFunction(catalog_ptr);

    String op = "count";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    SharedPtr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        DataType data_type(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(Boolean)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }
    {
        DataType data_type(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(TinyInt)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeTinyInt(i));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }
    {
        DataType data_type(LogicalType::kSmallInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(SmallInt)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeSmallInt(static_cast<SmallIntT>(i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }

    {
        DataType data_type(LogicalType::kInteger);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(Integer)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeInt(static_cast<IntegerT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }

    {
        DataType data_type(LogicalType::kBigInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(BigInt)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeBigInt(static_cast<BigIntT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }

    {
        DataType data_type(LogicalType::kHugeInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(HugeInt)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            HugeIntT input(0, i);
            Value v = Value::MakeHugeInt(input);
            data_block.AppendValue(0, v);
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }

    {
        DataType data_type(LogicalType::kFloat);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(Float)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeFloat(static_cast<FloatT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }

    {
        DataType data_type(LogicalType::kDouble);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("COUNT(Double)->BigInt", func.ToString().c_str());

        Vector<DataType> column_types;
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            data_block.AppendValue(0, Value::MakeDouble(static_cast<DoubleT>(2 * i)));
        }
        data_block.Finalize();

        func.init_func_(func.GetState());
        func.update_func_(func.GetState(), data_block.column_vectors[0]);
        BigIntT result;
        result = *(BigIntT*)func.finalize_func_(func.GetState());

        EXPECT_FLOAT_EQ(result, row_count);
    }
}
