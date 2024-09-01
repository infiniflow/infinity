#include "gtest/gtest.h"
import base_test;
import stl;
import simd_init;

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
