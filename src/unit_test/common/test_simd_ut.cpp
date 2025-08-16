
#ifdef CI
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_simd;

import :ut.base_test;
import :simd_init;
#endif

using namespace infinity;

class SimdInitTest : public BaseTest {};

TEST_F(SimdInitTest, GetSupportedSimdTypesList) {
    std::cout << "Supported SIMD types:";
    for (const auto &simd_type : infinity::GetSupportedSimdTypesList()) {
        std::cout << " " << simd_type;
    }
    std::cout << std::endl;
    // check endianess for src\common\simd\maxsim_simd_funcs.cppm:66
    alignas(alignof(u16)) u8 v[2] = {1, 0};
    EXPECT_EQ(*reinterpret_cast<const u16 *>(v), 1u);
}
