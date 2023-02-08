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
#include "function/aggregate/avg.h"
#include "function/aggregate_function_set.h"
#include "expression/column_expression.h"

class AvgFunctionTest : public BaseTest {
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

TEST_F(AvgFunctionTest, abs_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterAvgFunction(catalog_ptr);

    String op = "avg";
    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName(op);
    EXPECT_EQ(function_set->type_, FunctionType::kAggregate);
    SharedPtr<AggregateFunctionSet> aggregate_function_set = std::static_pointer_cast<AggregateFunctionSet>(function_set);
    {
        DataType data_type(LogicalType::kTinyInt);
        SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);

        AggregateFunction func = aggregate_function_set->GetMostMatchFunction(col_expr_ptr);
        EXPECT_STREQ("AVG(TinyInt)->Double", func.ToString().c_str());

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
            Value v = data_block.GetValue(0, i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(v.value_.tiny_int, static_cast<i8>(i));
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(data_type);
//        result->Initialize();
//        func.function_(data_block, result);
//
//        for (SizeT i = 0; i < row_count; ++i) {
//            Value v = result->GetValue(i);
//            EXPECT_EQ(v.type_.type(), LogicalType::kTinyInt);
//            i8 res = static_cast<i8>(i);
//            if (res == std::numeric_limits<i8>::min()) {
//                EXPECT_EQ(result->nulls_ptr_->IsTrue(i), false);
//            } else {
//                EXPECT_EQ(result->nulls_ptr_->IsTrue(i), true);
//                EXPECT_EQ(v.value_.tiny_int, abs(res));
//            }
//        }
    }
}