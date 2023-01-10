//
// Created by jinhai on 23-1-7.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"
#include "storage/catalog.h"
#include "function/scalar/or.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class OrFunctionsTest : public BaseTest {
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

TEST_F(OrFunctionsTest, or_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterOrFunction(catalog_ptr);

    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName("or");
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type(LogicalType::kBoolean);
        DataType result_type(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                "t1",
                                                                                "c1",
                                                                                0,
                                                                                0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(data_type,
                                                                                 "t1",
                                                                                 "c2",
                                                                                 1,
                                                                                 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("or(Boolean, Boolean)->Boolean", func.ToString().c_str());

        std::vector<DataType> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        size_t row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (size_t i = 0; i < row_count; ++i) {
            if(i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeBool(true));
                data_block.AppendValue(1, Value::MakeBool(false));
            } else {
                data_block.AppendValue(0, Value::MakeBool(false));
                data_block.AppendValue(1, Value::MakeBool(true));
            }
        }
        data_block.Finalize();

        for (size_t i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBoolean);
            if(i % 2 == 0) {
                EXPECT_EQ(v1.value_.boolean, true);
                EXPECT_EQ(v2.value_.boolean, false);
            } else {
                EXPECT_EQ(v1.value_.boolean, false);
                EXPECT_EQ(v2.value_.boolean, true);
            }
        }

        ColumnVector result(result_type);
        result.Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < row_count; ++i) {
            Value v = result.GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v.value_.boolean, true);
        }
    }
}