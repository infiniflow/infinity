//
// Created by JinHai on 2022/7/25.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/utility/str.h"

class StrTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

TEST_F(StrTest, test1) {
    std::string path = "/usr/src/storage/db";
    std::string trimed_path = infinity::TrimPath(path);
    ASSERT_EQ(trimed_path, "src/storage/db");
}
