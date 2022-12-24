//
// Created by jinhai on 22-12-24.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/geography_cast.h"
#include "common/types/info/varchar_info.h"

class CircleCastTest : public BaseTest {
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

TEST_F(CircleCastTest, circle_cast0) {
    using namespace infinity;

    // Try to cast circle type to wrong type.
    {
        PointT p1(1, 1);
        f64 r(2);
        CircleT source(p1, r);
        TinyIntT target;
        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        PointT p1(1, 1);
        f64 r(2);
        CircleT source(p1, r);
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
    }
}

TEST_F(CircleCastTest, circle_cast1) {
    using namespace infinity;

    // Call BindGeographyCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kCircle);
        DataType target_type(LogicalType::kDecimal16);
        EXPECT_THROW(BindGeographyCast<CircleT>(source_type, target_type), TypeException);
    }

    DataType source_type(LogicalType::kCircle);
    ColumnVector col_source(source_type, ColumnVectorType::kFlat);
    col_source.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        PointT p1(static_cast<f64>(i), static_cast<f64>(i));
        f64 r = static_cast<f64>(i);
        Value v = Value::MakeCircle(CircleT(p1, r));
        col_source.AppendValue(v);
        Value vx = col_source.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_source.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kCircle);
        EXPECT_FLOAT_EQ(vx.value_.circle.center.x, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.circle.center.y, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.circle.radius, static_cast<f64>(i));
    }
    // cast circle column vector to varchar column vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto source2target_ptr = BindGeographyCast<CircleT>(source_type, target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type, ColumnVectorType::kFlat);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}
