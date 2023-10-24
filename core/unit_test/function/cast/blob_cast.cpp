//
// Created by jinhai on 22-12-23.
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
import parser;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import cast_table;
import column_vector;
import blob_cast;
import bound_cast_func;

class BlobCastTest : public BaseTest {
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

TEST_F(BlobCastTest, blob_cast0) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // Try to cast blob type to wrong type.
    {
        i64 blob_len = 128;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT source(blob_ptr, blob_len);

        TinyIntT target;
        EXPECT_THROW(BlobTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        i64 blob_len = 128;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT source(blob_ptr, blob_len);
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_TRUE(BlobTryCastToVarlen::Run(source, target, col_varchar_ptr));
        target.Reset(false);
    }
}

TEST_F(BlobCastTest, blob_cast1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // Call BindBlobCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindBlobCast(target_type), TypeException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kBlob);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        col_source->AppendValue(v);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }
    // cast blob column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindBlobCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_TRUE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters));

        for (i64 i = 99; i < 100; ++i) {
            i64 blob_len = i + 1;
            auto check_value = String();
            check_value.resize(blob_len);

            for (i64 j = 0; j < blob_len; ++j) {
                check_value[j] = 'a' + static_cast<char_t>(j);
            }
            check_value[blob_len - 1] = 0;
            Value vx = col_target->GetValue(i);
            EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
            EXPECT_FALSE(vx.is_null());
            EXPECT_STREQ(vx.value_.varchar.ToString().c_str(), check_value.c_str());
        }
    }
}
