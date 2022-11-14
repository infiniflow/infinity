//
// Created by JinHai on 2022/11/14.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"

class CharTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
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
    c5.Initialize("bcdef");
    EXPECT_EQ(c5.ToString(), "bc");

    Char2T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bc");

    Char2T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bc");
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
    c5.Initialize("bcdef");
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
    c5.Initialize("bcdefghijk");
    EXPECT_EQ(c5.ToString(), "bcdefghi");

    Char8T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcdefghi");

    Char8T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcdefghi");
}

TEST_F(CharTypeTest, Char15) {
    using namespace infinity;

    String s = "bcdefghijklmnop";
    EXPECT_LE(s.length(), 15);

    Char15T c1(s);
    EXPECT_EQ(c1.ToString(), s);

    Char15T c2("bcdefghijklmnop");
    EXPECT_EQ(c2.ToString(), s);

    Char15T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), s);

    Char15T c4(c1);
    EXPECT_EQ(c4.ToString(), s);

    Char15T c5;
    c5.Initialize("bcdefghijklmnop", 14);
    EXPECT_EQ(c5.ToString(), "bcdefghijklmno");

    Char15T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcdefghijklmno");

    Char15T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcdefghijklmno");
}

TEST_F(CharTypeTest, Char31) {
    using namespace infinity;

    String s = "bcdefghijklmnop bcdefghijklmnop";
    EXPECT_LE(s.length(), 31);

    Char31T c1(s);
    EXPECT_EQ(c1.ToString(), s);

    Char31T c2("bcdefghijklmnop bcdefghijklmnop");
    EXPECT_EQ(c2.ToString(), s);

    Char31T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), s);

    Char31T c4(c1);
    EXPECT_EQ(c4.ToString(), s);

    Char31T c5;
    c5.Initialize("bcdefghijklmnop bcdefghijklmnop", 31);
    EXPECT_EQ(c5.ToString(), "bcdefghijklmnop bcdefghijklmnop");

    Char31T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcdefghijklmnop bcdefghijklmnop");

    Char31T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcdefghijklmnop bcdefghijklmnop");
}

TEST_F(CharTypeTest, Char63) {
    using namespace infinity;

    String s = "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop";
    EXPECT_LE(s.length(), 63);

    Char63T c1(s);
    EXPECT_EQ(c1.ToString(), s);

    Char63T c2("bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");
    EXPECT_EQ(c2.ToString(), s);

    Char63T c3(std::move(c1));
    EXPECT_EQ(c3.ToString(), s);

    Char63T c4(c1);
    EXPECT_EQ(c4.ToString(), s);

    Char63T c5;
    c5.Initialize("bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop", 63);
    EXPECT_EQ(c5.ToString(), "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");

    Char63T c6;
    c6 = c5;
    EXPECT_EQ(c6.ToString(), "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");

    Char63T c7;
    c7 = std::move(c5);
    EXPECT_EQ(c7.ToString(), "bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop bcdefghijklmnop");
}

