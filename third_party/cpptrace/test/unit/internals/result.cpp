#include <gtest/gtest.h>

#include "utils/result.hpp"

using cpptrace::detail::Result;

namespace {

// A simple custom error type that behaves like a standard exception.
struct error {
    int x;
    const char* what() const {
        return "error...";
    }
};

class ResultFixture : public testing::Test {
public:
    ResultFixture() {
        cpptrace::absorb_trace_exceptions(true);
    }

    ~ResultFixture() override {
        cpptrace::absorb_trace_exceptions(false);
    }
};

TEST_F(ResultFixture, ConstructWithValueRValue) {
    Result<std::string, error> result("test");
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.is_error());
    EXPECT_TRUE(static_cast<bool>(result));

    EXPECT_EQ(result.unwrap_value(), "test");
    EXPECT_FALSE(result.error().has_value());
}

TEST_F(ResultFixture, ConstructWithValueLValue) {
    std::string s = "test";
    Result<std::string, error> result(s);

    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.is_error());
    EXPECT_EQ(result.unwrap_value(), "test");

    s = "x";
    EXPECT_EQ(result.unwrap_value(), "test");

    Result<std::string&, error> r2(s);
    EXPECT_EQ(r2.unwrap_value(), "x");
    s = "y";
    EXPECT_EQ(r2.unwrap_value(), "y");
}

TEST_F(ResultFixture, ConstructWithErrorRValue) {
    Result<std::string, error> result(error{1});
    EXPECT_FALSE(result.has_value());
    EXPECT_TRUE(result.is_error());
    EXPECT_FALSE(static_cast<bool>(result));

    EXPECT_EQ(result.unwrap_error().x, 1);

    // Check that value() returns nullopt in this scenario
    EXPECT_FALSE(result.value().has_value());
}

TEST_F(ResultFixture, ConstructWithErrorLValue) {
    error e{1};
    Result<std::string, error> result(e);

    EXPECT_FALSE(result.has_value());
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.unwrap_error().x, 1);
}

TEST_F(ResultFixture, MoveConstructorValue) {
    Result<std::string, error> original(std::string("move"));
    Result<std::string, error> moved(std::move(original));
    EXPECT_TRUE(moved.has_value());
    EXPECT_EQ(moved.unwrap_value(), "move");
    EXPECT_TRUE(original.has_value());

    std::string s = "test";
    Result<std::string&, error> r1(s);
    Result<std::string&, error> r2(std::move(r1));
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(r2.unwrap_value(), "test");
    s = "foo";
    EXPECT_EQ(r2.unwrap_value(), "foo");
    EXPECT_TRUE(r2.has_value());
}

TEST_F(ResultFixture, MoveConstructorError) {
    Result<std::string, error> original(error{1});
    Result<std::string, error> moved(std::move(original));

    EXPECT_TRUE(moved.is_error());
    EXPECT_EQ(moved.unwrap_error().x, 1);
    EXPECT_TRUE(original.is_error());
}

TEST_F(ResultFixture, MoveAssignmentValue) {
    Result<std::string, error> original(std::string("move"));
    Result<std::string, error> target = std::string{};
    target = std::move(original);
    EXPECT_TRUE(target.has_value());
    EXPECT_EQ(target.unwrap_value(), "move");
    EXPECT_TRUE(original.has_value());

    std::string s = "test";
    std::string n;
    Result<std::string&, error> r1(s);
    Result<std::string&, error> r2(n);
    r2 = std::move(r1);
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(r2.unwrap_value(), "test");
    s = "foo";
    EXPECT_EQ(r2.unwrap_value(), "foo");
    EXPECT_TRUE(r2.has_value());
}

TEST_F(ResultFixture, MoveAssignmentError) {
    Result<std::string, error> original(error{1});
    Result<std::string, error> target = std::string{};
    target = std::move(original);

    EXPECT_TRUE(target.is_error());
    EXPECT_EQ(target.unwrap_error().x, 1);
    EXPECT_TRUE(original.is_error());
}

TEST_F(ResultFixture, CopyAssignmentValue) {
    Result<std::string, error> original(std::string("move"));
    Result<std::string, error> target = std::string{};
    target = original;
    EXPECT_TRUE(target.has_value());
    EXPECT_EQ(target.unwrap_value(), "move");
    EXPECT_TRUE(original.has_value());

    std::string s = "test";
    std::string n;
    Result<std::string&, error> r1(s);
    Result<std::string&, error> r2(n);
    r2 = r1;
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(r2.unwrap_value(), "test");
    s = "foo";
    EXPECT_EQ(r2.unwrap_value(), "foo");
    EXPECT_TRUE(r2.has_value());
}

TEST_F(ResultFixture, CopyAssignmentError) {
    Result<std::string, error> original(error{1});
    Result<std::string, error> target = std::string{};
    target = original;

    EXPECT_TRUE(target.is_error());
    EXPECT_EQ(target.unwrap_error().x, 1);
    EXPECT_TRUE(original.is_error());
}

TEST_F(ResultFixture, ValueOr) {
    {
        Result<int, error> res_with_value(42);
        EXPECT_EQ(res_with_value.value_or(-1), 42);
        EXPECT_EQ(std::move(res_with_value).value_or(-1), 42);
    }
    {
        Result<int, error> res_with_error(error{});
        EXPECT_EQ(res_with_error.value_or(-1), -1);
        EXPECT_EQ(std::move(res_with_error).value_or(-1), -1);
    }
    {
        int x = 2;
        int y = 3;
        Result<int&, error> res_with_value(x);
        EXPECT_EQ(res_with_value.value_or(y), 2);
        EXPECT_EQ(std::move(res_with_value).value_or(y), 2);
    }
    {
        int x = 2;
        Result<int&, error> res_with_error(error{});
        EXPECT_EQ(res_with_error.value_or(x), 2);
        EXPECT_EQ(&res_with_error.value_or(x), &x);
        EXPECT_EQ(std::move(res_with_error).value_or(x), 2);
    }
}

}
