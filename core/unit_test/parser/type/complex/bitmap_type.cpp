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

class BitmapTypeTest : public BaseTest {
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

TEST_F(BitmapTypeTest, bitmap1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // default constructor, Initialize, SetBit, GetBit
    BitmapT bt1;
    bt1.Initialize(100);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            bt1.SetBit(i, true);
        } else {
            bt1.SetBit(i, false);
        }
    }

    EXPECT_THROW(bt1.GetBit(100), ParserException);
    EXPECT_THROW(bt1.SetBit(100, false), ParserException);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt1.GetBit(i), true);
        } else {
            EXPECT_EQ(bt1.GetBit(i), false);
        }
    }

    // copy assignment1
    BitmapT bt2(100);
    bt2 = bt1;

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt2.GetBit(i), true);
        } else {
            EXPECT_EQ(bt2.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt1, bt2);
    EXPECT_FALSE(bt1 != bt2);

    // copy assignment2
    BitmapT bt3(50);

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            bt3.SetBit(i, true);
        } else {
            bt3.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt3.GetBit(i), true);
        } else {
            EXPECT_EQ(bt3.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt3.GetBit(50), ParserException);
    EXPECT_THROW(bt3.SetBit(50, false), ParserException);

    bt3 = bt1;

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt3.GetBit(i), true);
        } else {
            EXPECT_EQ(bt3.GetBit(i), false);
        }
    }

    BitmapT bt4 = std::move(bt3);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt4.GetBit(i), true);
        } else {
            EXPECT_EQ(bt4.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt3.ptr, nullptr);
    EXPECT_EQ(bt3.count, 0);

    // move assignment
    BitmapT bt5(50);

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            bt5.SetBit(i, true);
        } else {
            bt5.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt5.GetBit(i), true);
        } else {
            EXPECT_EQ(bt5.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt5.GetBit(50), ParserException);
    EXPECT_THROW(bt5.SetBit(50, false), ParserException);

    bt5 = std::move(bt2);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt5.GetBit(i), true);
        } else {
            EXPECT_EQ(bt5.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt2.ptr, nullptr);
    EXPECT_EQ(bt2.count, 0);

    // Constructor
    u64 *ptr = new u64[2]{};

    BitmapT bt6(ptr, 100);
    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            bt6.SetBit(i, true);
        } else {
            bt6.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt6.GetBit(i), true);
        } else {
            EXPECT_EQ(bt6.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt6.GetBit(100), ParserException);
    EXPECT_THROW(bt6.SetBit(100, false), ParserException);
}

TEST_F(BitmapTypeTest, bitmap2) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // default constructor, Initialize, SetBit, GetBit
    BitmapT bt1;
    bt1.Initialize(100);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            bt1.SetBit(i, true);
        } else {
            bt1.SetBit(i, false);
        }
    }

    EXPECT_THROW(bt1.GetBit(100), ParserException);
    EXPECT_THROW(bt1.SetBit(100, false), ParserException);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt1.GetBit(i), true);
        } else {
            EXPECT_EQ(bt1.GetBit(i), false);
        }
    }

    // copy assignment1
    BitmapT bt2(100);
    bt2 = bt1;

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt2.GetBit(i), true);
        } else {
            EXPECT_EQ(bt2.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt1, bt2);
    EXPECT_FALSE(bt1 != bt2);

    // copy assignment2
    BitmapT bt3(50);

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            bt3.SetBit(i, true);
        } else {
            bt3.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt3.GetBit(i), true);
        } else {
            EXPECT_EQ(bt3.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt3.GetBit(50), ParserException);
    EXPECT_THROW(bt3.SetBit(50, false), ParserException);

    bt3 = bt1;

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt3.GetBit(i), true);
        } else {
            EXPECT_EQ(bt3.GetBit(i), false);
        }
    }

    BitmapT bt4 = std::move(bt3);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt4.GetBit(i), true);
        } else {
            EXPECT_EQ(bt4.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt3.ptr, nullptr);
    EXPECT_EQ(bt3.count, 0);

    // move assignment
    BitmapT bt5(50);

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            bt5.SetBit(i, true);
        } else {
            bt5.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt5.GetBit(i), true);
        } else {
            EXPECT_EQ(bt5.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt5.GetBit(50), ParserException);
    EXPECT_THROW(bt5.SetBit(50, false), ParserException);

    bt5 = std::move(bt2);

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt5.GetBit(i), true);
        } else {
            EXPECT_EQ(bt5.GetBit(i), false);
        }
    }

    EXPECT_EQ(bt2.ptr, nullptr);
    EXPECT_EQ(bt2.count, 0);

    // Constructor
    u64 *ptr = new u64[2]{};

    BitmapT bt6(ptr, 100);
    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            bt6.SetBit(i, true);
        } else {
            bt6.SetBit(i, false);
        }
    }

    for (u64 i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            EXPECT_EQ(bt6.GetBit(i), true);
        } else {
            EXPECT_EQ(bt6.GetBit(i), false);
        }
    }

    EXPECT_THROW(bt6.GetBit(100), ParserException);
    EXPECT_THROW(bt6.SetBit(100, false), ParserException);
}

TEST_F(BitmapTypeTest, bitmap33) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // default constructor, Initialize, SetBit, GetBit
    BitmapT bt1;
    bt1.Initialize(100);

    // copy assignment1
    BitmapT bt2(100);
}

TEST_F(BitmapTypeTest, bitmap34) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    BitmapT bt11;
    bt11.Initialize(100);

    // copy assignment2
    BitmapT bt33(50);

    bt33 = bt11;

    //    BitmapT bt33 = bt11;

    // move assignment
    BitmapT bt55(50);
}