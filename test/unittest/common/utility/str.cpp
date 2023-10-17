//
// Created by JinHai on 2022/7/25.
//

#include "base_test.h"
#include "main/infinity.h"

class StrTest : public BaseTest {};

TEST_F(StrTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");
}
