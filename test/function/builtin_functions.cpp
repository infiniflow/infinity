//
// Created by jinhai on 22-12-24.
//
#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"
//#include "storage/catalog.h"
//#include "func/builtin_functions.h"

class BuiltinFunctionsTest : public BaseTest {
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

TEST_F(BuiltinFunctionsTest, test1) {
//    using namespace infinity;
//
//    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();
//
//    BuiltinFunctions builtin_functions(catalog_ptr);
//    builtin_functions.Init();
}