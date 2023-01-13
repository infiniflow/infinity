//
// Created by jinhai on 23-1-12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/char_info.h"

class ValueCastTest : public BaseTest {
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

TEST_F(ValueCastTest, bool_value_cast) {
    using namespace infinity;

    DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
    Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

    Value value = Value::MakeBool(true);
    {
        EXPECT_EQ(value.GetValue<BooleanT>(), true);
        value.TryCastAs(target_type, target_value);
        EXPECT_FALSE(target_value.is_null());
        EXPECT_EQ(target_value.type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(target_value.value_.varchar.ToString(), "true");
    }

    {
        value = Value::MakeBool(false);
        EXPECT_EQ(value.GetValue<BooleanT>(), false);
        value.TryCastAs(target_type, target_value);
        EXPECT_FALSE(target_value.is_null());
        EXPECT_EQ(target_value.type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(target_value.value_.varchar.ToString(), "false");
    }
}

TEST_F(ValueCastTest, tinyint_value_cast) {
    using namespace infinity;

    {
        // TinyInt to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }
    }

    {
        // TinyInt to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }
    }

    {
        // TinyInt to BigInt
        DataType target_type(LogicalType::kBigInt);
        Value target_value(LogicalType::kBigInt);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }
    }

    {
        // TinyInt to HugeInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<i8>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }
    }

    {
        // TinyInt to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }
    }

    {
        // TinyInt to Double
        DataType target_type(LogicalType::kDouble);
        Value target_value(LogicalType::kDouble);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = 0;
            value = Value::MakeTinyInt(source);
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }
    }

    {
        // TinyInt to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // TinyInt to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // TinyInt to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // TinyInt to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        TinyIntT source = std::numeric_limits<TinyIntT>::min();
        Value value = Value::MakeTinyInt(source);
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // TinyInt to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        Value value = Value::MakeTinyInt(std::numeric_limits<TinyIntT>::min());
        {
            EXPECT_EQ(value.GetValue<TinyIntT>(), std::numeric_limits<TinyIntT>::min());
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(std::numeric_limits<TinyIntT>::min()));
        }

        {
            value = Value::MakeTinyInt(std::numeric_limits<TinyIntT>::max());
            EXPECT_EQ(value.GetValue<TinyIntT>(), std::numeric_limits<TinyIntT>::max());
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(std::numeric_limits<TinyIntT>::max()));
        }

        {
            value = Value::MakeTinyInt(0);
            EXPECT_EQ(value.GetValue<TinyIntT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(0));
        }
    }
}

TEST_F(ValueCastTest, smallint_value_cast) {
    using namespace infinity;

    {
        // TinyInt to SmallInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<TinyIntT>::min();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

    }

    {
        // SmallInt to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }
    }

    {
        // SmallInt to BigInt
        DataType target_type(LogicalType::kBigInt);
        Value target_value(LogicalType::kBigInt);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }
    }

    {
        // SmallInt to BigInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<SmallIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<SmallIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<SmallIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }
    }

    {
        // SmallInt to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }
    }

    {
        // SmallInt to Double
        DataType target_type(LogicalType::kDouble);
        Value target_value(LogicalType::kDouble);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = 0;
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }
    }

    {
        // SmallInt to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // SmallInt to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // SmallInt to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // SmallInt to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // SmallInt to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        SmallIntT source = std::numeric_limits<SmallIntT>::min();
        Value value = Value::MakeSmallInt(source);
        {
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeSmallInt(source);
            EXPECT_EQ(value.GetValue<SmallIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            value = Value::MakeSmallInt(0);
            EXPECT_EQ(value.GetValue<SmallIntT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(0));
        }
    }
}

TEST_F(ValueCastTest, integer_value_cast) {
    using namespace infinity;

    {
        // Integer to TinyInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<TinyIntT>::min();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }
    }

    {
        // Integer to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::min();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }
    }

    {
        // Integer to BigInt
        DataType target_type(LogicalType::kBigInt);
        Value target_value(LogicalType::kBigInt);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kBigInt);
            EXPECT_EQ(target_value.value_.big_int, source);
        }
    }

    {
        // Integer to HugeInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<IntegerT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<IntegerT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<IntegerT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }
    }

    {
        // Integer to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }
    }

    {
        // Integer to Double
        DataType target_type(LogicalType::kDouble);
        Value target_value(LogicalType::kDouble);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = 0;
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }
    }

    {
        // IntegerT to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // IntegerT to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // IntegerT to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // IntegerT to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // Integer to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        IntegerT source = std::numeric_limits<IntegerT>::min();
        Value value = Value::MakeInt(source);
        {
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeInt(source);
            EXPECT_EQ(value.GetValue<IntegerT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            value = Value::MakeInt(0);
            EXPECT_EQ(value.GetValue<IntegerT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(0));
        }
    }
}
