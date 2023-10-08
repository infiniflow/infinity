//
// Created by JinHai on 2022/11/13.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"


class ValueTest : public BaseTest {
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

TEST_F(ValueTest, Bitmap) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    Value value = Value::MakeBool(true);

    for(int j = 0; j < 100; ++j) {
        {
            PointT point1(1.0f, 2.0f);
            CircleT circle(point1, 1.3f);
            value = Value::MakeCircle(circle);
            EXPECT_EQ(value.GetValue<CircleT>(), circle);
        }

        {
            BitmapT bt1;
            bt1.Initialize(100);

            for(u64 i = 0; i < 100; ++i) {
                if(i % 2 == 0) {
                    bt1.SetBit(i, true);
                } else {
                    bt1.SetBit(i, false);
                }
            }
            value = Value::MakeBitmap(bt1);
            EXPECT_EQ(value.GetValue<BitmapT>(), bt1);
        }
    }
}

TEST_F(ValueTest, Array) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    Value value = Value::MakeBool(true);

    {
        ArrayT array;
        array.reserve(3);

        array.emplace_back(Value::MakeVarchar("Hello"));
        array.emplace_back(Value::MakeTinyInt(12));
        array.emplace_back(Value::MakeFloat(3.2));

        value = Value::MakeArray(array);
        {
            ArrayT x = value.GetValue<ArrayT>();
            EXPECT_EQ(x.size(), 3);
        }
        EXPECT_EQ(value.GetValue<ArrayT>().size(), 3);
        EXPECT_EQ(value.GetValue<ArrayT>()[0].type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(value.GetValue<ArrayT>()[1].type_.type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.GetValue<ArrayT>()[2].type_.type(), LogicalType::kFloat);
    }

    {
        ArrayT array;
        array.reserve(3);

        array.emplace_back(Value::MakeVarchar("Hello"));
        array.emplace_back(Value::MakeTinyInt(12));
        array.emplace_back(Value::MakeFloat(3.2));

        value = Value::MakeArray(array);
        EXPECT_EQ(value.GetValue<ArrayT>().size(), 3);
        EXPECT_EQ(value.GetValue<ArrayT>()[0].type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(value.GetValue<ArrayT>()[1].type_.type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.GetValue<ArrayT>()[2].type_.type(), LogicalType::kFloat);
    }
}

TEST_F(ValueTest, Blob) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    Value value = Value::MakeBool(true);

    constexpr i64 SIZE = 27;

    auto blob_ptr = new char[SIZE]{0};
    GlobalResourceUsage::IncrRawMemCount();

    for(i64 i = 0; i < SIZE; ++i) {
        blob_ptr[i] = 'a' + static_cast<char_t>(i);
    }
    blob_ptr[SIZE - 1] = 0;

    BlobT b1(blob_ptr, SIZE);

    value = Value::MakeBlob(b1);
    EXPECT_EQ(value.GetValue<BlobT>(), b1);
}

TEST_F(ValueTest, MakeAndGet) {

    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    // Boolean
    Value value = Value::MakeBool(true);
    EXPECT_EQ(value.GetValue<BooleanT>(), true);

    value = Value::MakeBool(false);
    EXPECT_EQ(value.GetValue<BooleanT>(), false);

    // TinyInt
    for(i8 input = std::numeric_limits<i8>::min();; ++input) {
//        printf("%d\n", input);
        value = Value::MakeTinyInt(input);
        EXPECT_EQ(value.GetValue<TinyIntT>(), input);
        if(input == std::numeric_limits<i8>::max())
            break;
    }

    // SmallInt
    for(i16 input = std::numeric_limits<i16>::min();; ++input) {
//        printf("%d\n", input);
        value = Value::MakeSmallInt(input);
        EXPECT_EQ(value.GetValue<SmallIntT>(), input);
        if(input == std::numeric_limits<i16>::max())
            break;
    }

    // Integer
    for(i32 input = std::numeric_limits<i32>::min();; input += std::numeric_limits<u16>::max()) {
        value = Value::MakeInt(input);
        EXPECT_EQ(value.GetValue<IntegerT>(), input);
        if(input == std::numeric_limits<i32>::max())
            break;
    }

    // Big Integer
    value = Value::MakeBigInt(std::numeric_limits<i64>::min());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::min());

    value = Value::MakeBigInt(0);
    EXPECT_EQ(value.GetValue<BigIntT>(), 0);

    value = Value::MakeBigInt(std::numeric_limits<i64>::max());
    EXPECT_EQ(value.GetValue<BigIntT>(), std::numeric_limits<i64>::max());

    // Huge Integer
    HugeInt huge_int(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max());
    value = Value::MakeHugeInt(huge_int);
    EXPECT_EQ(value.GetValue<HugeIntT>(), HugeInt(std::numeric_limits<i64>::min(), std::numeric_limits<i64>::max()));

    // Float32
    value = Value::MakeFloat(0);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 0);
    value = Value::MakeFloat(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), -1.5);
    value = Value::MakeFloat(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<FloatT>(), 3.232);

    // Float64
    value = Value::MakeDouble(0);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 0);
    value = Value::MakeDouble(-1.5);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), -1.5);
    value = Value::MakeDouble(3.232);
    EXPECT_FLOAT_EQ(value.GetValue<DoubleT>(), 3.232);

    // Decimal
    DecimalT decimal_value(10000, 10001);
    SharedPtr<TypeInfo> type_info_ptr = DecimalInfo::Make(38, 38);
    value = Value::MakeDecimal(decimal_value, type_info_ptr);
    EXPECT_EQ(value.GetValue<DecimalT>(), decimal_value);

    // Varchar (inline)

    value = Value::MakeVarchar("Hello World!");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), true);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World!");

    // Varchar (heap allocation)
    value = Value::MakeVarchar("Hello World, Hello World");
    EXPECT_EQ(value.GetValue<VarcharT>().IsInlined(), false);
    EXPECT_EQ(value.GetValue<VarcharT>().ToString(), "Hello World, Hello World");

    // Date
    {
        DateT date;
        for(i32 i = 0; i < 365; ++i) {
            date.value = i;
            value = Value::MakeDate(date);
            EXPECT_EQ(value.GetValue<DateT>().value, i);
        }
    }

    // Time
    {
        TimeT time;
        for(i32 i = 0; i < 3600 * 24; ++i) {
            time.value = i;
            value = Value::MakeTime(time);
            EXPECT_EQ(value.GetValue<TimeT>().value, i);
        }
    }

    // Datetime
    {
        DateTimeT datetime;
        for(i32 i = 0; i < 365; ++i) {
            datetime.date = i;
            datetime.time = 3600 - i;
            value = Value::MakeDateTime(datetime);
            EXPECT_EQ(value.GetValue<DateTimeT>().date, i);
            EXPECT_EQ(value.GetValue<DateTimeT>().time, 3600 - i);
        }
    }

    // Timestamp
    {
        TimestampT timestamp;
        for(i32 i = 0; i < 365; ++i) {
            timestamp.date = i;
            timestamp.time = 3600 - i;
            value = Value::MakeTimestamp(timestamp);
            EXPECT_EQ(value.GetValue<TimestampT>().date, i);
            EXPECT_EQ(value.GetValue<TimestampT>().time, 3600 - i);
        }
    }

    // Interval
    {
        IntervalT interval;
        for(i32 unit = 1; unit < 7; ++unit) {
            interval.unit = TimeUnit::kSecond;
            for(i32 v = 1; v < 3600; ++v) {
                interval.value = v;
                value = Value::MakeInterval(interval);
                EXPECT_EQ(value.GetValue<IntervalT>().unit, TimeUnit::kSecond);
                EXPECT_EQ(value.GetValue<IntervalT>().value, v);
            }
        }
    }

    // Array
    {
        ArrayT array;
        array.emplace_back(Value::MakeVarchar("Hello"));
        array.emplace_back(Value::MakeTinyInt(12));
        array.emplace_back(Value::MakeFloat(3.2));
        array.emplace_back(Value::MakeVarchar("HelloHelloHelloHelloHello"));

        value = Value::MakeArray(array);
        EXPECT_EQ(value.GetValue<ArrayT>().size(), 4);
        EXPECT_EQ(value.GetValue<ArrayT>()[0].type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(value.GetValue<ArrayT>()[1].type_.type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.GetValue<ArrayT>()[2].type_.type(), LogicalType::kFloat);
        EXPECT_EQ(value.GetValue<ArrayT>()[3].type_.type(), LogicalType::kVarchar);
    }

    // Tuple
    {
        TupleT tuple;
        tuple.emplace_back(Value::MakeVarchar("Hello"));
        tuple.emplace_back(Value::MakeTinyInt(12));
        tuple.emplace_back(Value::MakeFloat(3.2));
        tuple.emplace_back(Value::MakeVarchar("HelloHelloHelloHelloHello"));

        value = Value::MakeTuple(tuple);
        EXPECT_EQ(value.GetValue<TupleT>().size(), 4);
        EXPECT_EQ(value.GetValue<TupleT>()[0].type_.type(), LogicalType::kVarchar);
        EXPECT_EQ(value.GetValue<TupleT>()[1].type_.type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.GetValue<TupleT>()[2].type_.type(), LogicalType::kFloat);
        EXPECT_EQ(value.GetValue<TupleT>()[3].type_.type(), LogicalType::kVarchar);
    }

    // PointT
    {
        PointT point1(1.0f, 2.0f);
        value = Value::MakePoint(point1);
        EXPECT_EQ(value.GetValue<PointT>(), point1);
    }

    // LineT
    {
        LineT line(1.0f, 2.0f, 3.0f);
        value = Value::MakeLine(line);
        EXPECT_EQ(value.GetValue<LineT>(), line);
    }

    // LineSegT
    {
        PointT point1(1.0f, 2.0f), point2(3.0f, -5.4f);
        LineSegT line_seg(point1, point2);
        value = Value::MakeLineSegment(line_seg);
        EXPECT_EQ(value.GetValue<LineSegT>(), line_seg);
    }

    // BoxT
    {
        PointT point1(1.0f, 2.0f), point2(3.0f, -5.4f);
        BoxT box(point1, point2);
        value = Value::MakeBox(box);
        EXPECT_EQ(value.GetValue<BoxT>(), box);
    }

    // Path
    {
        PathT path1;
        path1.Initialize(2);
        path1.SetPoint(0, PointT(1.0, 2.1));
        path1.SetPoint(1, PointT(1.1, 2.2));
        value = Value::MakePath(path1);
        EXPECT_EQ(value.GetValue<PathT>(), path1);
    }

    // Polygon
    {
        PolygonT polygon1;
        polygon1.Initialize(2);
        polygon1.SetPoint(0, PointT(1.0, 2.1));
        polygon1.SetPoint(1, PointT(1.1, 2.2));
        value = Value::MakePolygon(polygon1);
        EXPECT_EQ(value.GetValue<PolygonT>(), polygon1);
    }

    // Circle
    {
        PointT point1(1.0f, 2.0f);
        CircleT circle(point1, 1.3f);
        value = Value::MakeCircle(circle);
        EXPECT_EQ(value.GetValue<CircleT>(), circle);
    }

    // BitmapT
    {
        BitmapT bt1;
        bt1.Initialize(100);

        for(u64 i = 0; i < 100; ++i) {
            if(i % 2 == 0) {
                bt1.SetBit(i, true);
            } else {
                bt1.SetBit(i, false);
            }
        }
        value = Value::MakeBitmap(bt1);
        EXPECT_EQ(value.GetValue<BitmapT>(), bt1);
    }

    // UuidT
    {
        char uuid_str[17] = "aabbccddeeffgghh";

        // Default constructor and Set
        UuidType uuid1{};
        uuid1.Set(uuid_str);
        value = Value::MakeUuid(uuid1);
        EXPECT_EQ(value.GetValue<UuidT>(), uuid1);
    }

    // BlobT
    {
        constexpr i64 SIZE = 27;

        auto blob_ptr = new char[SIZE]{0};
        GlobalResourceUsage::IncrRawMemCount();

        for(i64 i = 0; i < SIZE; ++i) {
            blob_ptr[i] = 'a' + static_cast<char_t>(i);
        }
        blob_ptr[SIZE - 1] = 0;

        BlobT b1(blob_ptr, SIZE);

        value = Value::MakeBlob(b1);
        EXPECT_EQ(value.GetValue<BlobT>(), b1);
    }

    // EmbeddingT
    {
        value = Value::MakeEmbedding(EmbeddingDataType::kElemFloat, 128);
        EmbeddingT eb = value.GetValue<EmbeddingType>();
        EXPECT_EQ(eb.ptr, value.value_.embedding.ptr);
        // Embedding need to manually SetNull or Reset.
        value.value_.embedding.Reset();
        eb.SetNull();

        value = Value::MakeBool(true);
        EXPECT_EQ(value.value_.boolean, true);
    }

    // RowT
    {
        RowT row{1, 2, 3};
        value = Value::MakeRow(row);
        EXPECT_EQ(value.value_.row.segment_id_, 1);
        EXPECT_EQ(value.value_.row.block_id_, 2);
        EXPECT_EQ(value.value_.row.block_offset_, 3);
    }

    // Mixed Integer
    {
        MixedT mixed_integer1 = MixedType::MakeInteger(10);
        value = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_integer1);
    }

    // Mixed Float
    {
        MixedT mixed_float1 = MixedType::MakeFloat(10.05f);
        value = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_float1);
    }

    // Mixed Short string
    {
        MixedT mixed_short_str = MixedType::MakeString("hello");
        value = Value::MakeMixedData(mixed_short_str);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_short_str);
    }

    // Mixed Long string
    {
        MixedT mixed_long_str = MixedType::MakeString("hellohellohellohellohello");
        value = Value::MakeMixedData(mixed_long_str);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_long_str);
    }

    // Mixed Null
    {
        MixedT mixed_null = MixedType::MakeNull();
        value = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_null);
    }

    // Mixed Tuple
    {
        MixedType mixed_tuple1 = MixedType::MakeTuple(5);
        // Key1: integer
        mixed_tuple1.InsertIntegerIntoTuple("key1", 100);
        // Key2: float
        mixed_tuple1.InsertFloatIntoTuple("key2", 1.1);
        // Key3: short string
        mixed_tuple1.InsertStringIntoTuple("key3", "Hello World !!");
        // Key4: long string
        mixed_tuple1.InsertStringIntoTuple("key4", "Hello World Hello World");
        // Key5: null
        mixed_tuple1.InsertNullIntoTuple("key5");

        value = Value::MakeMixedData(mixed_tuple1);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_tuple1);
    }

    // Mixed Array
    {
        MixedType mixed_array7 = MixedType::MakeArray(5);
        // Integer
        mixed_array7.InsertIntegerIntoArray(4300, 0);
        // Float
        mixed_array7.InsertFloatIntoArray(5.5, 1);
        // Short str
        mixed_array7.InsertStringIntoArray("FunnyHalloween", 2);
        // Long str
        mixed_array7.InsertStringIntoArray("FunnyHalloween OK", 3);
        // Null
        mixed_array7.InsertNullIntoArray(4);

        value = Value::MakeMixedData(mixed_array7);
        EXPECT_EQ(value.GetValue<MixedT>(), mixed_array7);
    }
}