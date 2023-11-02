//
// Created by JinHai on 2022/7/25.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import infinity_context;

class TrimTest : public BaseTest {};

TEST_F(TrimTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");
}
