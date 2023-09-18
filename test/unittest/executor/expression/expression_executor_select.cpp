//
// Created by jinhai on 23-1-24.
//
#if 0
#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "executor/expression/expression_evaluator.h"

#include "storage/catalog.h"
#include "function/scalar/subtract.h"
#include "function/scalar/add.h"
#include "function/scalar_function_set.h"
#include "expression/column_expression.h"

class ExpressionExecutorSelectTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ExpressionExecutorSelectTest, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionSelector::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE / 2);
    for(SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++ idx) {
        EXPECT_EQ((*output_true_select)[idx], idx * 2);
    }
}

TEST_F(ExpressionExecutorSelectTest, test2) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(true);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, true);
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE);
    for(SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE; ++ idx) {
        EXPECT_EQ((*output_true_select)[idx], idx);
    }
}

TEST_F(ExpressionExecutorSelectTest, test3) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(false);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, false);
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    EXPECT_EQ(output_true_select->Size(), 0);
    EXPECT_THROW((*output_true_select)[0], ExecutorException);
}
#endif
