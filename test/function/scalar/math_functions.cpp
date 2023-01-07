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

class MathFunctionsTest : public BaseTest {
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

TEST_F(MathFunctionsTest, test1) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterAbsFunction(catalog_ptr);

    SharedPtr<FunctionSet> function_set = catalog_ptr->GetFunctionSetByName("abs");
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    Vector<SharedPtr<BaseExpression>> inputs;

    SharedPtr<ColumnExpression> col_expr_ptr = MakeShared<ColumnExpression>(DataType(LogicalType::kTinyInt),
                                                                            "t1",
                                                                            "c1",
                                                                            0,
                                                                            0);

    inputs.emplace_back(col_expr_ptr);
//    EXPECT_THROW(scalar_function_set->GetMostMatchFunction(inputs), PlannerException);
    ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
    LOG_TRACE("{}", func.ToString());
}