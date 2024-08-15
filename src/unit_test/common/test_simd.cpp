#include "unit_test/base_test.h"
#include <iostream>
#include <vector>

import stl;
import simd_init;

class SimdInitTest : public BaseTest {};

using namespace infinity;

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
