//
// Created by jinhai on 22-12-22.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/decimal_cast.h"
#include "common/types/info/varchar_info.h"

class Decimal128CastTest : public BaseTest {
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

TEST_F(Decimal128CastTest, decimal128_cast0) {
    using namespace infinity;

    // Decimal128T to TinyIntT, throw exception
    {
        Decimal128T source;
        TinyIntT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to SmallIntT, throw exception
    {
        Decimal128T source;
        SmallIntT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to IntegerT, throw exception
    {
        Decimal128T source;
        IntegerT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to BigIntT, throw exception
    {
        Decimal128T source;
        BigIntT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to HugeIntT, throw exception
    {
        Decimal128T source;
        HugeIntT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to FloatT, throw exception
    {
        Decimal128T source;
        FloatT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to DoubleT, throw exception
    {
        Decimal128T source;
        DoubleT target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to Decimal16T, throw exception
    {
        Decimal128T source;
        Decimal16T target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to Decimal32T, throw exception
    {
        Decimal128T source;
        Decimal32T target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to Decimal64T, throw exception
    {
        Decimal128T source;
        Decimal64T target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), NotImplementException);
    }

    // Decimal128T to Decimal128T, throw exception
    {
        Decimal128T source;
        Decimal128T target;
        EXPECT_THROW(DecimalTryCastToFixlen::Run(source, target), FunctionException);
    }

    // Decimal128T to VarcharT, throw exception
    {
        Decimal128T source;
        VarcharT target;
        EXPECT_THROW(DecimalTryCastToVarlen::Run(source, target, nullptr), NotImplementException);
    }
}

TEST_F(Decimal128CastTest, decimal128_cast1) {
    using namespace infinity;

    // cast decimal128 column vector to tiny int column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kTinyInt);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to small int column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kSmallInt);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to integer column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kInteger);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to bigint column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kBigInt);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to hugeint column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kHugeInt);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to float column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kFloat);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to double column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kDouble);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to decimal16 column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kDecimal16);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to decimal32 column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kDecimal32);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to decimal128 column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kDecimal64);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }
    // cast decimal128 column vector to decimal128 column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kDecimal128);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), TypeException);
    }
    // cast decimal128 column vector to varchar column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kVarchar);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), NotImplementException);
    }

    // cast decimal128 column vector to other type column vector
    {
        DataType source_type(LogicalType::kDecimal128);
        DataType target_type(LogicalType::kTimestamp);
        EXPECT_THROW(BindDecimalCast<Decimal128T>(source_type, target_type), TypeException);
    }
}