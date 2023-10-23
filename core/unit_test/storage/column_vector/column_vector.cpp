//
// Created by JinHai on 2022/12/4.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import logger;
import column_vector;
import value;
import parser;
import default_values;
import third_party;
import stl;
import global_resource_usage;
import infinity;

class ColumnVectorTest : public BaseTest {
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

TEST_F(ColumnVectorTest, ReadWrite) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    Vector<SharedPtr<ColumnVector>> columns;
    int vector_size = DEFAULT_VECTOR_SIZE / 3;
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kBoolean));
        column->Initialize();
        for (i64 i = 0; i < vector_size; ++i) {
            Value v = Value::MakeBool(bool(i % 2));
            column->AppendValue(v);
        }
        columns.push_back(column);
    }
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kFloat));
        column->Initialize();
        for (i64 i = 0; i < vector_size; ++i) {
            Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
            column->AppendValue(v);
        }
        columns.push_back(column);
    }
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kFloat));
        column->Initialize(ColumnVectorType::kConstant);
        for (i64 i = 0; i < 1; ++i) {
            Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
            column->AppendValue(v);
        }
        columns.push_back(column);
    }

    /*
    // TODO: Following case cause exception due to the value created by `Value::MakeBitmap` has null info_ptr.
         {
            SharedPtr<ColumnVector> column =
                ColumnVector::Make(MakeShared<DataType>(LogicalType::kBitmap,
       BitmapInfo::Make(1024))); column->Initialize(); for (i64 i = 0; i <
       vector_size; ++i) { BitmapT bitmap; bitmap.Initialize(i + 10); for (i64 j
       = 0; j <= i; ++j) { if (j % 2 == 0) { bitmap.SetBit(j, true); } else {
                        bitmap.SetBit(j, false);
                    }
                }
                Value v = Value::MakeBitmap(bitmap);
                column->AppendValue(v);
            }
            columns.push_back(column);
        }
     */
    for (int i = 0; i < columns.size(); i++) {
        int32_t exp_size = columns[i]->GetSizeInBytes();
        int32_t act_size;
        Vector<char> buf(exp_size);
        char *ptr = buf.data();

        columns[i]->WriteAdv(ptr);
        act_size = ptr - buf.data();
        EXPECT_EQ(act_size, exp_size);

        ptr = buf.data();
        SharedPtr<ColumnVector> column2 = ColumnVector::ReadAdv(ptr, exp_size);
        act_size = ptr - buf.data();
        EXPECT_EQ(act_size, exp_size);
        EXPECT_NE(column2, nullptr);
        EXPECT_EQ(*columns[i] == *column2, true);
    }
}
