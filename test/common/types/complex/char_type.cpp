//
// Created by JinHai on 2022/11/14.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class CharTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(CharTypeTest, Char1) {
    using namespace infinity;

    Char1T char1;
    char1.Initialize('a');
    EXPECT_EQ(char1.value, 'a');

    Char1T char2(char1);
    EXPECT_EQ(char1.value, char2.value);

    String s = "bcd";
    Char1T char3(s);
    EXPECT_EQ(char3.value, 'b');

    Char1T char4(s.c_str());
    EXPECT_EQ(char4.value, 'b');

    Char1T char5;
    char5 = char1;
    EXPECT_EQ(char5.value, char1.value);
}

TEST_F(CharTypeTest, Char2) {
    using namespace infinity;

    String s = "bcd";

    Char2T c1(s);
    EXPECT_EQ(c1.ToString(), "bc");

    Char2T c2("bcdef");
    EXPECT_EQ(c2.ToString(), "bc");

    Char2T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), "bc");

    Char2T c4(c1);
    EXPECT_EQ(c4.ToString(), "bc");

    Char2T c5;
    c5.Initialize("bcdef", 2);
    EXPECT_EQ(c5.ToString(), "bc");

    Char2T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bc");

    Char2T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bc");

    s = "b";
    Char2T c8;
    c8.Initialize(s.c_str(), s.length());
    EXPECT_EQ(c8.ToString(), "b");
}

TEST_F(CharTypeTest, Char4) {
    using namespace infinity;

    String s = "bcdef";

    Char4T c1(s);
    EXPECT_EQ(c1.ToString(), "bcde");

    Char4T c2("bcdef");
    EXPECT_EQ(c2.ToString(), "bcde");

    Char4T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), "bcde");

    Char4T c4(c1);
    EXPECT_EQ(c4.ToString(), "bcde");

    Char4T c5;
    c5.Initialize("bcdef", 4);
    EXPECT_EQ(c5.ToString(), "bcde");

    Char4T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcde");

    Char4T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcde");
}

TEST_F(CharTypeTest, Char8) {
    using namespace infinity;

    String s = "bcdefghijk";

    Char8T c1(s);
    EXPECT_EQ(c1.ToString(), "bcdefghi");

    Char8T c2("bcdefghijk");
    EXPECT_EQ(c2.ToString(), "bcdefghi");

    Char8T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), "bcdefghi");

    Char8T c4(c1);
    EXPECT_EQ(c4.ToString(), "bcdefghi");

    Char8T c5;
    c5.Initialize("bcdefghijk", 8);
    EXPECT_EQ(c5.ToString(), "bcdefghi");

    Char8T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcdefghi");

    Char8T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcdefghi");
}

TEST_F(CharTypeTest, Char16) {
    using namespace infinity;

    String s = "bcdefghijklmnop";
    EXPECT_LE(s.length(), Char16Type::CHAR_LENGTH);

    Char16T c1(s);
    String result = c1.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char16T c2("bcdefghijklmnop");
    result = c2.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char16T c3(std::move(c1));
    result = c3.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char16T c4(c1);
    result = c4.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char16T c5;
    c5.Initialize("bcdefghijklmnop", 14);
    result = c5.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmno");

    Char16T c6;
    c6 = c5;
    result = c6.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmno");

    Char16T c7;
    c7 = std::move(c5);
    result = c7.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmno");
}

TEST_F(CharTypeTest, Char32) {
    using namespace infinity;

    String s = "bcdefghijklmnop bcdefghijklmnop";
    EXPECT_LE(s.length(), 31);

    Char32T c1(s);
    String result = c1.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char32T c2("bcdefghijklmnop bcdefghijklmnop");
    result = c2.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char32T c3(std::move(c1));
    result = c3.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char32T c4(c1);
    result = c4.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char32T c5;
    c5.Initialize("bcdefghijklmnop bcdefghijklmnop", 31);
    result = c5.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop");

    Char32T c6;
    c6 = c5;
    result = c6.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop");

    Char32T c7;
    c7 = std::move(c5);
    result = c7.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop");
}

TEST_F(CharTypeTest, Char64) {
    using namespace infinity;

    String s = "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop";
    EXPECT_LE(s.length(), 63);

    Char64T c1(s);
    String result = c1.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char64T c2("bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");
    result = c2.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char64T c3(std::move(c1));
    result = c3.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char64T c4(c1);
    result = c4.ToString();
    trim(result);
    EXPECT_EQ(result, s);

    Char64T c5;
    c5.Initialize("bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop", 63);
    result = c5.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");

    Char64T c6;
    c6 = c5;
    result = c6.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");

    Char64T c7;
    c7 = std::move(c5);
    result = c1.ToString();
    trim(result);
    EXPECT_EQ(result, "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");
}

