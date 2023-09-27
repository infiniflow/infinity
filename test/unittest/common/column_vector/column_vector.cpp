//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "common/utility/serializable.h"

class ColumnVectorTest : public BaseTest {
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

TEST_F(ColumnVectorTest, ReadWrite) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
    Vector<SharedPtr<ColumnVector>> columns = {ColumnVector::Make(data_type), ColumnVector::Make(data_type)};
    columns[0]->Initialize();
    columns[1]->Initialize(ColumnVectorType::kConstant);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        columns[0]->AppendValue(v);
    }
    columns[1]->AppendValue(Value::MakeFloat(0.123));

    for(int i=0; i<columns.size(); i++){
        int32_t exp_size = columns[i]->GetSizeInBytes();
        Vector<char> buf(exp_size);
        char *ptr = buf.data();

        columns[i]->WriteAdv(ptr);
        EXPECT_EQ(ptr - buf.data(), exp_size);

        ptr = buf.data();
        SharedPtr<ColumnVector> column2 = ColumnVector::ReadAdv(ptr, exp_size);
        EXPECT_EQ(ptr - buf.data(), exp_size);
        EXPECT_NE(column2, nullptr);
        EXPECT_EQ(*columns[i]==*column2, true);
    }
}
