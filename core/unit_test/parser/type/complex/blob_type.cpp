//
// Created by JinHai on 2022/11/16.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;

class BlobTypeTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(BlobTypeTest, blob1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    constexpr i64 SIZE = 27;

    auto blob_ptr = new char[SIZE]{0};

    for (i64 i = 0; i < SIZE; ++i) {
        blob_ptr[i] = 'a' + static_cast<char_t>(i);
    }
    blob_ptr[SIZE - 1] = 0;

    EXPECT_STREQ(blob_ptr, "abcdefghijklmnopqrstuvwxyz");

    // Constructor from ptr and size
    BlobT b1(blob_ptr, SIZE);
    EXPECT_STREQ(b1.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b1.size, SIZE);

    // Copy constructor
    BlobT b2(b1);
    EXPECT_STREQ(b2.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b2.size, SIZE);

    EXPECT_EQ(b1, b2);
    EXPECT_FALSE(b1 != b2);

    // Move constructor
    BlobT b3(std::move(b1));
    EXPECT_STREQ(b2.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b2.size, SIZE);

    EXPECT_EQ(b1.ptr, nullptr);
    EXPECT_EQ(b1.size, 0);

    // Copy assignment
    BlobT b4;
    b4 = b2;
    EXPECT_STREQ(b4.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b4.size, SIZE);

    // Move assignment
    BlobT b5;
    b5 = std::move(b2);
    EXPECT_STREQ(b5.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b5.size, SIZE);

    EXPECT_EQ(b2.ptr, nullptr);
    EXPECT_EQ(b2.size, 0);

    // Reset;
    b5.Reset();
    EXPECT_EQ(b5.ptr, nullptr);
    EXPECT_EQ(b5.size, 0);

    // Move from ptr;
    blob_ptr = new char[SIZE]{0};

    for (i64 i = 0; i < SIZE; ++i) {
        blob_ptr[i] = 'a' + static_cast<char_t>(i);
    }
    blob_ptr[SIZE - 1] = 0;
    BlobT b6;
    b6.Move(blob_ptr, SIZE);
    EXPECT_STREQ(b6.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b6.size, SIZE);

    // Copy from ptr;
    BlobT b7;
    b7.Copy(blob_ptr, SIZE);
    EXPECT_STREQ(b7.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b7.size, SIZE);
}
