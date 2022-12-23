//
// Created by jinhai on 22-12-23.
//


#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "function/cast/geography_cast.h"
#include "common/types/info/varchar_info.h"

class LineCastTest : public BaseTest {
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

TEST_F(LineCastTest, point_cast0) {
    using namespace infinity;

    // Try to cast line type to wrong type.
    {
        LineT source(1, 1, 1);
        TinyIntT target;
        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        LineT source(1, 1, 1);
        VarcharT target;

        auto varchar_info = VarcharInfo::Make(65);
        DataType data_type(LogicalType::kVarchar, varchar_info);
        ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
        col_varchar.Initialize();

        EXPECT_THROW(GeographyTryCastToVarlen::Run(source, target, &col_varchar), NotImplementException);
    }
}


TEST_F(LineCastTest, line_cast1) {
    using namespace infinity;

    // Call BindGeographyCast with wrong type of parameters
    {
        DataType source_type(LogicalType::kLine);
        DataType target_type(LogicalType::kDecimal16);
        EXPECT_THROW(BindGeographyCast<LineT>(source_type, target_type), TypeException);
    }

    DataType source_type(LogicalType::kLine);
    ColumnVector col_source(source_type, ColumnVectorType::kFlat);
    col_source.Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeLine(LineT(static_cast<f64>(i), static_cast<f64>(i), static_cast<f64>(i)));
        col_source.AppendValue(v);
        Value vx = col_source.GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_source.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kLine);
        EXPECT_FLOAT_EQ(vx.value_.line.a, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.line.b, static_cast<f64>(i));
        EXPECT_FLOAT_EQ(vx.value_.line.c, static_cast<f64>(i));
    }
    // cast line column vector to varchar column vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto source2target_ptr = BindGeographyCast<LineT>(source_type, target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        ColumnVector col_target(target_type, ColumnVectorType::kFlat);
        col_target.Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}