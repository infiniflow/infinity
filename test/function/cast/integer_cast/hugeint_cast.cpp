//
// Created by jinhai on 22-12-21.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/integer_cast.h"
#include "common/types/info/varchar_info.h"

class HugeIntCastTest : public BaseTest {
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

TEST_F(HugeIntCastTest, hugeint_cast0) {
    using namespace infinity;

    // HugeInt to HugeInt, throw exception
    {
        HugeIntT source{0, 0};
        HugeIntT target{0, 0};

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }

    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        TinyIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        SmallIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        IntegerT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        BigIntT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        FloatT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        DoubleT target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal16T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal32T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal64T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        Decimal128T target;
        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), NotImplementException);
    }
    {
        HugeIntT source(std::numeric_limits<i64>::max(), std::numeric_limits<i64>::max());
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        EXPECT_THROW(IntegerTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
    }
    // HugeInt to Date, throw exception
    {
        HugeIntT source{0, 0};
        DateT target{0};

        EXPECT_THROW(IntegerTryCastToFixlen::Run(source, target), FunctionException);
    }
}