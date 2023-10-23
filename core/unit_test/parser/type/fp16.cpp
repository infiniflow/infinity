#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;

class FP16TypeTest : public BaseTest {
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


TEST_F(FP16TypeTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    const uint16_t min_po2_f16 = UINT16_C(0x0400);
    const uint16_t eighths_f16 = UINT16_C(0x3000);
    const uint16_t quarter_f16 = UINT16_C(0x3400);
    const uint16_t half_f16 = UINT16_C(0x3800);
    const uint16_t one_f16 = UINT16_C(0x3C00);
    const uint16_t two_f16 = UINT16_C(0x4000);
    const uint16_t four_f16 = UINT16_C(0x4400);
    const uint16_t eight_f16 = UINT16_C(0x4800);
    const uint16_t sixteen_f16 = UINT16_C(0x4C00);
    const uint16_t thirtytwo_f16 = UINT16_C(0x5000);
    const uint16_t sixtyfour_f16 = UINT16_C(0x5400);
    const uint16_t max_po2_f16 = UINT16_C(0x7800);

    const uint32_t min_po2_f32 = UINT32_C(0x38800000);
    const uint32_t eighths_f32 = UINT32_C(0x3E000000);
    const uint32_t quarter_f32 = UINT32_C(0x3E800000);
    const uint32_t half_f32 = UINT32_C(0x3F000000);
    const uint32_t one_f32 = UINT32_C(0x3F800000);
    const uint32_t two_f32 = UINT32_C(0x40000000);
    const uint32_t four_f32 = UINT32_C(0x40800000);
    const uint32_t eight_f32 = UINT32_C(0x41000000);
    const uint32_t sixteen_f32 = UINT32_C(0x41800000);
    const uint32_t thirtytwo_f32 = UINT32_C(0x42000000);
    const uint32_t sixtyfour_f32 = UINT32_C(0x42800000);
    const uint32_t max_po2_f32 = UINT32_C(0x47000000);
    float min_po2_value;
    memcpy(&min_po2_value, &min_po2_f32, sizeof(min_po2_value));
    float16_t fp16 = (float16_t)min_po2_value;
    EXPECT_EQ((float16_t)min_po2_f16, fp16);

    float eighths_value;
    memcpy(&eighths_value, &eighths_f32, sizeof(eighths_value));
    fp16 = (float16_t)eighths_value;
    EXPECT_EQ((float16_t)eighths_f16, fp16);

    float quarter_value;
    memcpy(&quarter_value, &quarter_f32, sizeof(quarter_value));
    fp16 = (float16_t)quarter_value;
    EXPECT_EQ((float16_t)quarter_f16, fp16);

    float half_value;
    memcpy(&half_value, &half_f32, sizeof(half_value));
    fp16 = (float16_t)half_value;
    EXPECT_EQ((float16_t)half_f16, fp16);

    float one_value;
    memcpy(&one_value, &one_f32, sizeof(one_value));
    fp16 = (float16_t)one_value;
    EXPECT_EQ((float16_t)one_f16, fp16);

    float two_value;
    memcpy(&two_value, &two_f32, sizeof(two_value));
    fp16 = (float16_t)two_value;
    EXPECT_EQ((float16_t)two_f16, fp16);

    float four_value;
    memcpy(&four_value, &four_f32, sizeof(four_value));
    fp16 = (float16_t)four_value;
    EXPECT_EQ((float16_t)four_f16, fp16);

    float eight_value;
    memcpy(&eight_value, &eight_f32, sizeof(eight_value));
    fp16 = (float16_t)eight_value;
    EXPECT_EQ((float16_t)eight_f16, fp16);

    float sixteen_value;
    memcpy(&sixteen_value, &sixteen_f32, sizeof(sixteen_value));
    fp16 = (float16_t)sixteen_value;
    EXPECT_EQ((float16_t)sixteen_f16, fp16);

    float thirtytwo_value;
    memcpy(&thirtytwo_value, &thirtytwo_f32, sizeof(thirtytwo_value));
    fp16 = (float16_t)thirtytwo_value;
    EXPECT_EQ((float16_t)thirtytwo_f16, fp16);

    float sixtyfour_value;
    memcpy(&sixtyfour_value, &sixtyfour_f32, sizeof(sixtyfour_value));
    fp16 = (float16_t)sixtyfour_value;
    EXPECT_EQ((float16_t)sixtyfour_f16, fp16);

    float max_po2_value;
    memcpy(&max_po2_value, &max_po2_f32, sizeof(max_po2_value));
    fp16 = (float16_t)max_po2_value;
    EXPECT_EQ((float16_t)max_po2_f16, fp16);
}