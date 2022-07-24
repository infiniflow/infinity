//
// Created by JinHai on 2022/7/25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/utility/str.h"

class StrTest : public BaseTest {

};

TEST_F(StrTest, test1) {
    std::string path = "/usr/src/engine/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/engine/db");
}
