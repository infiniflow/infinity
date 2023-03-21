//
// Created by jinhai on 22-12-23.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "function/cast/bitmap_cast.h"


class BitmapCastTest : public BaseTest {
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

TEST_F(BitmapCastTest, bitmap_cast0) {
    using namespace infinity;

    // Try to cast bitmap type to wrong type.
    {
        BitmapT source;
        source.Initialize(128);
        for(i64 j = 0; j < 128; ++ j) {
            if(j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }

        TinyIntT target;
        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, nullptr), FunctionException);
    }
    {
        BitmapT source;
        source.Initialize(128);
        for(i64 j = 0; j < 128; ++ j) {
            if(j % 2 == 0) {
                source.SetBit(j, true);
            } else {
                source.SetBit(j, false);
            }
        }
        VarcharT target;


        DataType data_type(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_THROW(BitmapTryCastToVarlen::Run(source, target, col_varchar_ptr), NotImplementException);
    }
}


TEST_F(BitmapCastTest, bitmap_cast1) {
    using namespace infinity;

    // Call BindBitmapCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindBitmapCast(target_type), TypeException);
    }

    DataType source_type(LogicalType::kBitmap);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value v = Value::MakeBitmap(bitmap);
        col_source->AppendValue(v);
        Value vx = col_source->GetValue(i);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        BitmapT bitmap;
        bitmap.Initialize(i + 10);
        for(i64 j = 0; j <= i; ++ j) {
            if(j % 2 == 0) {
                bitmap.SetBit(j, true);
            } else {
                bitmap.SetBit(j, false);
            }
        }
        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBitmap);
        EXPECT_EQ(vx.value_.bitmap, bitmap);
    }
    // cast bitmap column vector to varchar column vector
    {
        DataType target_type(LogicalType::kVarchar);
        auto source2target_ptr = BindBitmapCast(target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_THROW(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters), NotImplementException);
    }
}
