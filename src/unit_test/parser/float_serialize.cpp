#include "unit_test/base_test.h"
#include <cassert>
#include <charconv>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>

import infinity_exception;

import third_party;
import stl;
import data_type;

class FloatSerializeTest : public BaseTest {};

TEST_F(FloatSerializeTest, test_precision) {
    using namespace infinity;
    constexpr f32 f1 = 0x1.fffffcp+9;
    constexpr f32 f2 = 0x1.fffffep+9;
//    // show << outputs
//    std::cout << "<< outputs" << std::endl;
//    std::cout << "f1: " << f1 << std::endl;
//    std::cout << "f2: " << f2 << std::endl;
//    // show printf outputs
//    std::cout << "printf(%f) outputs" << std::endl;
//    printf("f1: %f\n", f1);
//    printf("f2: %f\n", f2);
//    std::cout << "printf(%e) outputs" << std::endl;
//    printf("f1: %e\n", f1);
//    printf("f2: %e\n", f2);
//    std::cout << "printf(%g) outputs" << std::endl;
//    printf("f1: %g\n", f1);
//    printf("f2: %g\n", f2);
//    // show fmt::format outputs
//    std::cout << "fmt::format outputs" << std::endl;
//    std::cout << fmt::format("f1: {}\n", f1);
//    std::cout << fmt::format("f2: {}\n", f2);
//    // show std::to_chars outputs
    char buf[30];
//    std::cout << "std::to_chars outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
//    std::cout << "f32 max_digits10: " << std::numeric_limits<f32>::max_digits10 << std::endl;
    EXPECT_EQ(std::numeric_limits<f32>::max_digits10, 9);
//    std::cout << "f64 max_digits10: " << std::numeric_limits<f64>::max_digits10 << std::endl;
    EXPECT_EQ(std::numeric_limits<f64>::max_digits10, 17);
//    std::cout << "std::to_chars f32 precision 8 outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1, std::chars_format::general, 8);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2, std::chars_format::general, 8);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover != f2);
        EXPECT_NE(f2, recover);
    }
    // show std::to_chars scientific outputs
//    std::cout << "std::to_chars scientific outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1, std::chars_format::scientific);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2, std::chars_format::scientific);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
    // show std::to_chars fixed outputs
//    std::cout << "std::to_chars fixed outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1, std::chars_format::fixed);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2, std::chars_format::fixed);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
    // show std::to_chars hex outputs
//    std::cout << "std::to_chars hex outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1, std::chars_format::hex);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string("0x") + std::string(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2, std::chars_format::hex);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string("0x") + std::string(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
    // show std::to_chars general outputs
//    std::cout << "std::to_chars general outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1, std::chars_format::general);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2, std::chars_format::general);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
}

TEST_F(FloatSerializeTest, test_json) {
    using namespace infinity;
    constexpr f32 f1 = 0x1.fffffcp+9;
    constexpr f32 f2 = 0x1.fffffep+9;
    constexpr f64 f1_f64 = f1;
    constexpr f64 f2_f64 = f2;
    // show std::to_chars outputs
    char buf[30];
//    std::cout << "std::to_chars outputs" << std::endl;
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
        std::cout << "f1: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f1);
        EXPECT_EQ(f1, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2: " << buf << std::endl;
        const f32 recover = DataType::StringToValue<f32>(std::string_view(buf, p - buf));
        assert(recover == f2);
        EXPECT_EQ(f2, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f1_f64);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f1_f64: " << buf << std::endl;
        const f64 recover = DataType::StringToValue<f64>(std::string_view(buf, p - buf));
        assert(recover == f1_f64);
        EXPECT_EQ(f1_f64, recover);
    }
    {
        auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), f2_f64);
        EXPECT_EQ(ec, std::errc());
        *p = '\0';
//        std::cout << "f2_f64: " << buf << std::endl;
        const f64 recover = DataType::StringToValue<f64>(std::string_view(buf, p - buf));
        assert(recover == f2_f64);
        EXPECT_EQ(f2_f64, recover);
    }
    nlohmann::json j;
    j["f1"] = f1;
    j["f2"] = f2;
    EXPECT_EQ(j["f1"].get<f32>(), f1);
    EXPECT_EQ(j["f2"].get<f32>(), f2);
    // output json
//    std::cout << j.dump(4) << std::endl;
}

TEST_F(FloatSerializeTest, test_json_hex_float) {
    using namespace infinity;
    constexpr f32 f1 = 0x1.fffffcp+9;
    constexpr f32 f2 = 0x1.fffffep+9;
    const char *hex_float_json_invalid = R"(
{ "f1": 0x1.fffffcp+9, "f2": 0x1.fffffep+9 }
)";
    EXPECT_THROW((void)nlohmann::json::parse(hex_float_json_invalid), nlohmann::json::parse_error);
    const char *hex_float_json_valid_1 = R"(
{ "f1": 1023.9999, "f2": 1023.99994 }
)";
    nlohmann::json j_1 = nlohmann::json::parse(hex_float_json_valid_1);
    std::cout << j_1.dump(4) << std::endl;
    EXPECT_EQ(j_1["f1"].get<f32>(), f1);
    EXPECT_EQ(j_1["f2"].get<f32>(), f2);
    const char *hex_float_json_valid_2 = R"(
{ "f1": 1.0239999e+3, "f2": 1.02399994e+3 }
)";
    nlohmann::json j_2 = nlohmann::json::parse(hex_float_json_valid_2);
    std::cout << j_2.dump(4) << std::endl;
    EXPECT_EQ(j_2["f1"].get<f32>(), f1);
    EXPECT_EQ(j_2["f2"].get<f32>(), f2);
    const char *hex_float_json_valid_3 = R"(
{ "f1": 1, "f2": 1.0 }
)";
    nlohmann::json j_3 = nlohmann::json::parse(hex_float_json_valid_3);
    std::cout << j_3.dump(4) << std::endl;
    EXPECT_EQ(j_3["f1"].get<f32>(), 1.0f);
    EXPECT_EQ(j_3["f2"].get<f32>(), 1.0f);
}
