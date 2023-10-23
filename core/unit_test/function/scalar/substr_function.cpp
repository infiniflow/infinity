//
// Created by jinhai on 23-3-20.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;
import new_catalog;
import scalar_function;
import scalar_function_set;
import function_set;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import base_expression;
import column_vector;
import value_expression;
import substring;

class SubstrFunctionTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(SubstrFunctionTest, varchar_substr) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    UniquePtr<NewCatalog> catalog_ptr = MakeUnique<NewCatalog>(nullptr);

    RegisterSubstringFunction(catalog_ptr);

    {
        String op = "substring";
        SharedPtr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
        EXPECT_EQ(function_set->type_, FunctionType::kScalar);
        SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type0 = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnExpression> col0_expr_ptr = MakeShared<ColumnExpression>(*data_type0, "t1", 1, "c1", 0, 0);

        Value v1 = Value::MakeBigInt(1);
        SharedPtr<ValueExpression> pos_value_expr = MakeShared<ValueExpression>(v1);

        Value v2 = Value::MakeBigInt(3);
        SharedPtr<ValueExpression> len_value_expr = MakeShared<ValueExpression>(v2);

        inputs.emplace_back(col0_expr_ptr);
        inputs.emplace_back(pos_value_expr);
        inputs.emplace_back(len_value_expr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("substring(Varchar, BigInt, BigInt)->Varchar", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type0);
        column_types.emplace_back(MakeShared<DataType>(pos_value_expr->Type()));
        column_types.emplace_back(MakeShared<DataType>(len_value_expr->Type()));

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        SharedPtr<ColumnVector> col0 = ColumnVector::Make(data_type0);
        col0->Initialize(ColumnVectorType::kFlat);

        SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnVector> col1 = ColumnVector::Make(bigint_type);
        col1->Initialize(ColumnVectorType::kConstant);
        pos_value_expr->AppendToChunk(col1);
        SharedPtr<ColumnVector> col2 = ColumnVector::Make(bigint_type);
        col2->Initialize(ColumnVectorType::kConstant);
        len_value_expr->AppendToChunk(col2);

        for (SizeT idx = 0; idx < row_count; ++idx) {
            String s = "hello" + std::to_string(idx);
            VarcharT varchar_value(s);
            Value v = Value::MakeVarchar(varchar_value);
            col0->AppendValue(v);
            Value vx = col0->GetValue(idx);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            if (vx.value_.varchar.IsInlined()) {
                String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
                EXPECT_STREQ(prefix.c_str(), s.c_str());
            } else {
                String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
                EXPECT_STREQ(whole_str.c_str(), s.c_str());
            }
        }

        DataBlock data_block;
        data_block.Init({col0, col1, col2});
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT idx = 0; idx < row_count; ++idx) {
            Value vx = result->GetValue(idx);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "ell");
        }
    }

    {
        String op = "substring";
        SharedPtr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
        EXPECT_EQ(function_set->type_, FunctionType::kScalar);
        SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type0 = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnExpression> col0_expr_ptr = MakeShared<ColumnExpression>(*data_type0, "t1", 1, "c1", 0, 0);

        Value v1 = Value::MakeBigInt(15);
        SharedPtr<ValueExpression> pos_value_expr = MakeShared<ValueExpression>(v1);

        Value v2 = Value::MakeBigInt(4);
        SharedPtr<ValueExpression> len_value_expr = MakeShared<ValueExpression>(v2);

        inputs.emplace_back(col0_expr_ptr);
        inputs.emplace_back(pos_value_expr);
        inputs.emplace_back(len_value_expr);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("substring(Varchar, BigInt, BigInt)->Varchar", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type0);
        column_types.emplace_back(MakeShared<DataType>(pos_value_expr->Type()));
        column_types.emplace_back(MakeShared<DataType>(len_value_expr->Type()));

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        SharedPtr<ColumnVector> col0 = ColumnVector::Make(data_type0);
        col0->Initialize(ColumnVectorType::kFlat);

        SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
        SharedPtr<ColumnVector> col1 = ColumnVector::Make(bigint_type);
        col1->Initialize(ColumnVectorType::kConstant);
        pos_value_expr->AppendToChunk(col1);
        SharedPtr<ColumnVector> col2 = ColumnVector::Make(bigint_type);
        col2->Initialize(ColumnVectorType::kConstant);
        len_value_expr->AppendToChunk(col2);

        for (SizeT idx = 0; idx < row_count; ++idx) {
            String s = "hellohellohellohello" + std::to_string(idx);
            VarcharT varchar_value(s);
            Value v = Value::MakeVarchar(varchar_value);
            col0->AppendValue(v);
            Value vx = col0->GetValue(idx);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_FALSE(vx.value_.varchar.IsInlined());
            if (vx.value_.varchar.IsInlined()) {
                String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
                EXPECT_STREQ(prefix.c_str(), s.c_str());
            } else {
                String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
                EXPECT_STREQ(whole_str.c_str(), s.c_str());
            }
        }

        DataBlock data_block;
        data_block.Init({col0, col1, col2});
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT idx = 0; idx < row_count; ++idx) {
            Value vx = result->GetValue(idx);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), "hell");
        }
    }
}
