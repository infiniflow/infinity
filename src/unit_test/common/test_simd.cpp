#include "unit_test/base_test.h"
#include <iostream>
#include <vector>

import simd_init;

class SimdInitTest : public BaseTest {};

TEST_F(SimdInitTest, GetSupportedSimdTypesList) {
    std::cout << "Supported SIMD types:";
    for (const auto &simd_type : infinity::GetSupportedSimdTypesList()) {
        std::cout << " " << simd_type;
    }
    std::cout << std::endl;
}
