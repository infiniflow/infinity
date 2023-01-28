//
// Created by jinhai on 23-1-12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/char_info.h"

class ValueCastTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
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

TEST_F(ValueCastTest, bigint_value_cast) {
    using namespace infinity;

    {
        // BigInt to TinyInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<TinyIntT>::min();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }
    }

    {
        // BigInt to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::min();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }
    }

    {
        // BigInt to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<IntegerT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = std::numeric_limits<IntegerT>::min();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }


        {
            source = std::numeric_limits<BigIntT>::min();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }
    }

    {
        // BigInt to HugeInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<BigIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<BigIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kHugeInt);
            EXPECT_EQ(target_value.value_.huge_int.lower, source);
            EXPECT_EQ((static_cast<BigIntT>(source) < 0) * -1, target_value.value_.huge_int.upper);
        }
    }

    {
        // Integer to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
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

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = 0;
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }
    }

    {
        // BigIntT to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        BigIntT source = std::numeric_limits<BigIntT>::min();
        Value value = Value::MakeBigInt(source);
        {
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            source = std::numeric_limits<BigIntT>::max();
            value = Value::MakeBigInt(source);
            EXPECT_EQ(value.GetValue<BigIntT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            value = Value::MakeBigInt(0);
            EXPECT_EQ(value.GetValue<BigIntT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(0));
        }
    }
}

TEST_F(ValueCastTest, hugeint_value_cast) {
    using namespace infinity;

    {
        // HugeInt to TinyInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigInt to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeInt to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeInt to BigInt
        DataType target_type(LogicalType::kBigInt);
        Value target_value(LogicalType::kBigInt);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeInt to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeInt to Double
        DataType target_type(LogicalType::kDouble);
        Value target_value(LogicalType::kDouble);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeIntT to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // BigIntT to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // HugeIntT to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        HugeIntT source = std::numeric_limits<HugeIntT>::min();
        Value value = Value::MakeHugeInt(source);
        {
            EXPECT_EQ(value.GetValue<HugeIntT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }
}

TEST_F(ValueCastTest, float_value_cast) {
    using namespace infinity;

    {
        // Float to TinyInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        FloatT source = std::numeric_limits<FloatT>::lowest();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<TinyIntT>::min();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<FloatT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }
    }

    {
        // Float to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        FloatT source = std::numeric_limits<FloatT>::lowest();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::min();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<FloatT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }
    }

    {
        // Float to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        FloatT source = std::numeric_limits<FloatT>::lowest();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<IntegerT>::max() - 100;
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = std::numeric_limits<IntegerT>::min();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }


        {
            source = std::numeric_limits<FloatT>::lowest();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }
    }

    {
        // FloatT to HugeInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // FloatT to Double
        DataType target_type(LogicalType::kDouble);
        Value target_value(LogicalType::kDouble);

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = std::numeric_limits<FloatT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }

        {
            source = 0;
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kDouble);
            EXPECT_FLOAT_EQ(target_value.value_.float64, source);
        }
    }

    {
        // FloatT to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // FloatT to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // FloatT to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // FloatT to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        FloatT source = std::numeric_limits<FloatT>::lowest();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // FloatT to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        FloatT source = std::numeric_limits<FloatT>::min();
        Value value = Value::MakeFloat(source);
        {
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            source = std::numeric_limits<FloatT>::max();
            value = Value::MakeFloat(source);
            EXPECT_EQ(value.GetValue<FloatT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            value = Value::MakeFloat(0);
            EXPECT_EQ(value.GetValue<FloatT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(FloatT(0)));
        }
    }
}

TEST_F(ValueCastTest, double_value_cast) {
    using namespace infinity;

    {
        // Float to TinyInt
        DataType target_type(LogicalType::kTinyInt);
        Value target_value(LogicalType::kTinyInt);

        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<TinyIntT>::min();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<TinyIntT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }

        {
            source = std::numeric_limits<DoubleT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kTinyInt);
            EXPECT_EQ(target_value.value_.tiny_int, static_cast<TinyIntT>(source));
        }
    }

    {
        // Float to SmallInt
        DataType target_type(LogicalType::kSmallInt);
        Value target_value(LogicalType::kSmallInt);

        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<SmallIntT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<SmallIntT>::min();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }

        {
            source = std::numeric_limits<DoubleT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kSmallInt);
            EXPECT_EQ(target_value.value_.small_int, source);
        }
    }

    {
        // Float to Integer
        DataType target_type(LogicalType::kInteger);
        Value target_value(LogicalType::kInteger);

        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = std::numeric_limits<IntegerT>::max() - 100;
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }

        {
            source = std::numeric_limits<IntegerT>::min();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }


        {
            source = std::numeric_limits<DoubleT>::lowest();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_TRUE(target_value.is_null());
        }

        {
            source = 0;
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kInteger);
            EXPECT_EQ(target_value.value_.integer, source);
        }
    }

    {
        // DoubleT to HugeInt
        DataType target_type(LogicalType::kHugeInt);
        Value target_value(LogicalType::kHugeInt);

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // DoubleT to Float
        DataType target_type(LogicalType::kFloat);
        Value target_value(LogicalType::kFloat);

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = std::numeric_limits<DoubleT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }

        {
            source = 0;
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_FALSE(target_value.is_null());
            EXPECT_EQ(target_value.type_.type(), LogicalType::kFloat);
            EXPECT_FLOAT_EQ(target_value.value_.float32, source);
        }
    }

    {
        // DoubleT to Decimal16
        DataType target_type(LogicalType::kDecimal16);
        Value target_value(LogicalType::kDecimal16);

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // DoubleT to Decimal32
        DataType target_type(LogicalType::kDecimal32);
        Value target_value(LogicalType::kDecimal32);

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // DoubleT to Decimal64
        DataType target_type(LogicalType::kDecimal64);
        Value target_value(LogicalType::kDecimal64);

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // DoubleT to Decimal128
        DataType target_type(LogicalType::kDecimal128);
        Value target_value(LogicalType::kDecimal128);

        DoubleT source = std::numeric_limits<DoubleT>::lowest();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            EXPECT_THROW(value.TryCastAs(target_type, target_value), NotImplementException);
        }
    }

    {
        // DoubleT to Varchar
        DataType target_type(LogicalType::kVarchar, VarcharInfo::Make(64));
        Value target_value(LogicalType::kVarchar, VarcharInfo::Make(64));

        DoubleT source = std::numeric_limits<DoubleT>::min();
        Value value = Value::MakeDouble(source);
        {
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            source = std::numeric_limits<DoubleT>::max();
            value = Value::MakeDouble(source);
            EXPECT_EQ(value.GetValue<DoubleT>(), source);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(source));
        }

        {
            value = Value::MakeDouble(0);
            EXPECT_EQ(value.GetValue<DoubleT>(), 0);
            value.TryCastAs(target_type, target_value);
            EXPECT_EQ(target_value.value_.varchar.ToString(), std::to_string(DoubleT(0)));
        }
    }
}
