// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.value;

import :ut.base_test;
import :json_manager;

using namespace infinity;

class ValueStringTest : public BaseTest {};

TEST_F(ValueStringTest, test_tostring) {
    using namespace infinity;

    // Test Boolean
    {
        Value true_val = Value::MakeBool(true);
        EXPECT_EQ(true_val.ToString(), "true");

        Value false_val = Value::MakeBool(false);
        EXPECT_EQ(false_val.ToString(), "false");
    }

    // Test TinyInt
    {
        Value val = Value::MakeTinyInt(127);
        EXPECT_EQ(val.ToString(), "127");

        Value neg_val = Value::MakeTinyInt(-128);
        EXPECT_EQ(neg_val.ToString(), "-128");
    }

    // Test SmallInt
    {
        Value val = Value::MakeSmallInt(32767);
        EXPECT_EQ(val.ToString(), "32767");

        Value neg_val = Value::MakeSmallInt(-32768);
        EXPECT_EQ(neg_val.ToString(), "-32768");
    }

    // Test Integer
    {
        Value val = Value::MakeInt(2147483647);
        EXPECT_EQ(val.ToString(), "2147483647");

        Value neg_val = Value::MakeInt(-2147483648);
        EXPECT_EQ(neg_val.ToString(), "-2147483648");
    }

    // Test BigInt
    {
        Value val = Value::MakeBigInt(9223372036854775807LL);
        EXPECT_EQ(val.ToString(), "9223372036854775807");

        Value neg_val = Value::MakeBigInt(-9223372036854775807LL - 1);
        EXPECT_EQ(neg_val.ToString(), "-9223372036854775808");
    }

    // Test Float
    {
        Value val = Value::MakeFloat(3.14159f);
        // Float to string conversion may have rounding, just check it's not empty
        EXPECT_EQ(val.ToString(), "3.14159");

        Value neg_val = Value::MakeFloat(-2.71828f);
        EXPECT_EQ(neg_val.ToString(), "-2.71828");
    }

    // Test Double
    {
        Value val = Value::MakeDouble(3.141592653589793);
        EXPECT_EQ(val.ToString(), "3.141592653589793");

        Value neg_val = Value::MakeDouble(-2.718281828459045);
        EXPECT_EQ(neg_val.ToString(), "-2.718281828459045");
    }

    // Test Varchar
    {
        Value val = Value::MakeVarchar("Hello, World!");
        EXPECT_EQ(val.ToString(), "Hello, World!");

        Value empty_val = Value::MakeVarchar("");
        EXPECT_EQ(empty_val.ToString(), "");
    }

    // Test JSON object
    {
        auto json = nlohmann::json::parse("{\"key\": \"value\", \"number\": 42}");
        auto bson = JsonManager::to_bson(json);
        Value val = Value::MakeJson(bson, nullptr);

        std::string json_str = val.ToString();
        auto parsed_json = nlohmann::json::parse(json_str);
        EXPECT_EQ(parsed_json, json);
    }
}

TEST_F(ValueStringTest, test_stringtovalue) {
    using namespace infinity;

    // Test Boolean
    {
        Value true_val = Value::StringToValue("true", DataType(LogicalType::kBoolean));
        EXPECT_EQ(true_val.GetValue<BooleanT>(), true);

        Value false_val = Value::StringToValue("false", DataType(LogicalType::kBoolean));
        EXPECT_EQ(false_val.GetValue<BooleanT>(), false);
    }

    // Test TinyInt
    {
        Value val = Value::StringToValue("127", DataType(LogicalType::kTinyInt));
        EXPECT_EQ(val.GetValue<TinyIntT>(), 127);

        Value neg_val = Value::StringToValue("-128", DataType(LogicalType::kTinyInt));
        EXPECT_EQ(neg_val.GetValue<TinyIntT>(), -128);
    }

    // Test SmallInt
    {
        Value val = Value::StringToValue("32767", DataType(LogicalType::kSmallInt));
        EXPECT_EQ(val.GetValue<SmallIntT>(), 32767);

        Value neg_val = Value::StringToValue("-32768", DataType(LogicalType::kSmallInt));
        EXPECT_EQ(neg_val.GetValue<SmallIntT>(), -32768);
    }

    // Test Integer
    {
        Value val = Value::StringToValue("2147483647", DataType(LogicalType::kInteger));
        EXPECT_EQ(val.GetValue<IntegerT>(), 2147483647);

        Value neg_val = Value::StringToValue("-2147483648", DataType(LogicalType::kInteger));
        EXPECT_EQ(neg_val.GetValue<IntegerT>(), -2147483648);
    }

    // Test BigInt
    {
        Value val = Value::StringToValue("9223372036854775807", DataType(LogicalType::kBigInt));
        EXPECT_EQ(val.GetValue<BigIntT>(), 9223372036854775807LL);

        Value neg_val = Value::StringToValue("-9223372036854775808", DataType(LogicalType::kBigInt));
        EXPECT_EQ(neg_val.GetValue<BigIntT>(), -9223372036854775807LL - 1);
    }

    // Test Float
    {
        Value val = Value::StringToValue("3.14159", DataType(LogicalType::kFloat));
        EXPECT_FLOAT_EQ(val.GetValue<FloatT>(), 3.14159f);

        Value neg_val = Value::StringToValue("-2.71828", DataType(LogicalType::kFloat));
        EXPECT_FLOAT_EQ(neg_val.GetValue<FloatT>(), -2.71828f);
    }

    // Test Double
    {
        Value val = Value::StringToValue("3.141592653589793", DataType(LogicalType::kDouble));
        EXPECT_DOUBLE_EQ(val.GetValue<DoubleT>(), 3.141592653589793);

        Value neg_val = Value::StringToValue("-2.718281828459045", DataType(LogicalType::kDouble));
        EXPECT_DOUBLE_EQ(neg_val.GetValue<DoubleT>(), -2.718281828459045);
    }

    // Test Varchar
    {
        Value val = Value::StringToValue("Hello, World!", DataType(LogicalType::kVarchar));
        EXPECT_EQ(val.ToString(), "Hello, World!");

        Value empty_val = Value::StringToValue("", DataType(LogicalType::kVarchar));
        EXPECT_EQ(empty_val.ToString(), "");
    }

    // Test Date
    {
        Value val = Value::StringToValue("2023-12-25", DataType(LogicalType::kDate));
        DateT expected_date;
        expected_date.FromString("2023-12-25");
        EXPECT_EQ(val.GetValue<DateT>(), expected_date);
    }

    // Test Time
    {
        Value val = Value::StringToValue("14:30:45", DataType(LogicalType::kTime));
        TimeT expected_time;
        expected_time.FromString("14:30:45");
        EXPECT_EQ(val.GetValue<TimeT>(), expected_time);
    }

    // Test DateTime
    {
        Value val = Value::StringToValue("2023-12-25 14:30:45", DataType(LogicalType::kDateTime));
        DateTimeT expected_datetime;
        expected_datetime.FromString("2023-12-25 14:30:45");
        EXPECT_EQ(val.GetValue<DateTimeT>(), expected_datetime);
    }

    // Test Timestamp
    {
        Value val = Value::StringToValue("2023-12-25 14:30:45", DataType(LogicalType::kTimestamp));
        TimestampT expected_timestamp;
        expected_timestamp.FromString("2023-12-25 14:30:45");
        EXPECT_EQ(val.GetValue<TimestampT>(), expected_timestamp);
    }

    // Test JSON object
    {
        Value val = Value::StringToValue("{\"key\": \"value\", \"number\": 42}", DataType(LogicalType::kJson));
        // Parse and stringify to compare
        auto json = nlohmann::json::parse("{\"key\": \"value\", \"number\": 42}");
        auto bson = JsonManager::to_bson(json);
        Value expected_val = Value::MakeJson(bson, nullptr);
        EXPECT_EQ(val, expected_val);
    }
}