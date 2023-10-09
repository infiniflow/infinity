#include <gtest/gtest.h>

#include "base_test.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "storage/invertedindex/format/posting_value.h"
#include "storage/invertedindex/format/short_buffer.h"
#include <iostream>

namespace infinity {

template <typename T>
class SimpleFormat : public PostingValues {
public:
    SimpleFormat(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            PostingValue *value = new TypedPostingValue<T>();
            value->location_ = i;
            value->offset_ = i * sizeof(T);
            values_.push_back(value);
        }
    }
};

class ShortBufferTest : public BaseTest {
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

protected:
    void CheckShortBuffer(uint32_t *expect_short_buffer, size_t row_num, size_t col_num, ShortBuffer *short_buffer = NULL, uint8_t *capacity = NULL);

    void CheckEqual(uint32_t *expect_short_buffer, size_t row_num, size_t col_num, ShortBuffer *short_buffer);
};

void ShortBufferTest::CheckShortBuffer(uint32_t *expect_short_buffer, size_t row_num, size_t col_num, ShortBuffer *short_buffer, uint8_t *capacity) {
    std::unique_ptr<ShortBuffer> short_buffer_ptr;
    SimpleFormat<uint32_t> posting_values(row_num);
    if (!short_buffer) {
        short_buffer = new ShortBuffer;
        short_buffer_ptr.reset(short_buffer);
        short_buffer_ptr->Init(&posting_values);
    }
    uint8_t cur_capacity = capacity == NULL ? 0 : *capacity;
    ASSERT_EQ(cur_capacity, short_buffer->Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer->Size());
    ASSERT_EQ(row_num, short_buffer->GetRowCount());

    for (size_t c = 0; c < col_num; ++c) {
        for (size_t r = 0; r < row_num; ++r) {
            ASSERT_TRUE(short_buffer->PushBack(r, expect_short_buffer[c + r * col_num]));
        }
        short_buffer->EndPushBack();
        ASSERT_EQ((uint8_t)(c + 1), short_buffer->Size());
        uint8_t cur_capacity = capacity == NULL ? short_buffer->AllocatePlan(c) : *capacity;
        ASSERT_EQ(cur_capacity, short_buffer->Capacity());
    }

    CheckEqual(expect_short_buffer, row_num, col_num, short_buffer);
}

void ShortBufferTest::CheckEqual(uint32_t *expect_short_buffer, size_t row_num, size_t col_num, ShortBuffer *short_buffer) {
    ASSERT_EQ(row_num, (size_t)short_buffer->GetRowCount());
    ASSERT_EQ(col_num, (size_t)short_buffer->Size());
    for (size_t r = 0; r < row_num; ++r) {
        uint32_t *row = short_buffer->GetRowTyped<uint32_t>(r);
        for (size_t c = 0; c < col_num; ++c) {
            ASSERT_EQ(expect_short_buffer[c + r * col_num], row[c]);
            uint32_t *row = short_buffer->GetRowTyped<uint32_t>(r);
            ASSERT_EQ(expect_short_buffer[c + r * col_num], row[c]);
        }
    }
}

TEST_F(ShortBufferTest, test1) {
    SimpleFormat<uint32_t> posting_values(1);
    ShortBuffer short_buffer;
    short_buffer.Init(&posting_values);

    ASSERT_EQ((uint8_t)0, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack(0, (uint32_t)100));
    short_buffer.EndPushBack();
    ASSERT_EQ((uint8_t)2, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test2) {
    uint32_t expect_short_buffer[] = {100, 101, 102, 200, 201, 202};
    CheckShortBuffer((uint32_t *)expect_short_buffer, 2, 3);
}

TEST_F(ShortBufferTest, test3) {
    ShortBuffer short_buffer;
    SimpleFormat<uint32_t> posting_values(8);
    short_buffer.Init(&posting_values);

    uint32_t expect_short_buffer[128 * 8];
    for (size_t c = 0; c < 128; ++c) {
        for (size_t r = 0; r < 8; ++r) {
            expect_short_buffer[c + r * 128] = random();
        }
    }
    CheckShortBuffer((uint32_t *)expect_short_buffer, 8, 128, &short_buffer);

    short_buffer.Clear();

    uint8_t capcity = 128;
    CheckShortBuffer((uint32_t *)expect_short_buffer, 8, 128, &short_buffer, &capcity);
}

TEST_F(ShortBufferTest, test4) {
    {
        // push same row repeatedly
        ShortBuffer short_buffer;
        SimpleFormat<uint32_t> posting_values(2);
        short_buffer.Init(&posting_values);

        ASSERT_TRUE(short_buffer.PushBack<uint32_t>(0, 100));
        short_buffer.EndPushBack();
        ASSERT_TRUE(short_buffer.PushBack<uint32_t>(0, 200));
    }
    {
        // push when short_buffer is full and hit max capacity
        ShortBuffer short_buffer;
        SimpleFormat<uint32_t> posting_values(1);
        short_buffer.Init(&posting_values);

        for (size_t col = 0; col < 128; ++col) {
            ASSERT_TRUE(short_buffer.PushBack<uint32_t>(0, col));
            short_buffer.EndPushBack();
        }
        ASSERT_TRUE(!short_buffer.PushBack<uint32_t>(0, 128));
    }
}

TEST_F(ShortBufferTest, test5) {
    ShortBuffer short_buffer;
    SimpleFormat<uint32_t> posting_values(2);
    short_buffer.Init(&posting_values);

    ASSERT_TRUE(short_buffer.PushBack<uint32_t>(0, 100));
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<uint32_t>(1, 200));
    short_buffer.EndPushBack();
    ASSERT_EQ((uint8_t)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test6) {
    ShortBuffer short_buffer;
    SimpleFormat<uint32_t> posting_values(3);
    short_buffer.Init(&posting_values);

    ASSERT_EQ((uint8_t)0, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<uint32_t>(2, 100));

    ASSERT_EQ((uint8_t)2, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<uint32_t>(0, 100));

    ASSERT_EQ((uint8_t)2, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<uint32_t>(1, 100));
    short_buffer.EndPushBack();
    ASSERT_EQ((uint8_t)2, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test7) {
    ShortBuffer short_buffer;
    SimpleFormat<uint32_t> posting_values(1);
    short_buffer.Init(&posting_values);

    short_buffer.PushBack<uint32_t>(0, 100);
    short_buffer.EndPushBack();
    ASSERT_EQ(1, short_buffer.Size());
    ASSERT_EQ(2, short_buffer.Capacity());

    short_buffer.Clear();
    ASSERT_EQ(0, short_buffer.Size());
    ASSERT_EQ(2, short_buffer.Capacity());
}

TEST_F(ShortBufferTest, test8) {

// TODO some type order might fail when using memory pool
#define NUMBER_TYPE_HELPER_FOR_TEST(macro)                                                                                                           \
    macro(uint32_t);                                                                                                                                 \
    macro(uint32_t);                                                                                                                                 \
    macro(uint16_t);                                                                                                                                 \
    macro(uint16_t);

    PostingValues posting_values;
    size_t offset = 0;
    size_t pos = 0;

#define ADD_VALUE_HELPER_FOR_TEST(type)                                                                                                              \
    {                                                                                                                                                \
        PostingValue *value = new TypedPostingValue<type>();                                                                                         \
        value->location_ = pos++;                                                                                                                    \
        value->offset_ = offset;                                                                                                                     \
        offset += sizeof(type);                                                                                                                      \
        posting_values.AddValue(value);                                                                                                              \
    }

    NUMBER_TYPE_HELPER_FOR_TEST(ADD_VALUE_HELPER_FOR_TEST);

    ShortBuffer short_buffer;
    short_buffer.Init(&posting_values);

    ASSERT_EQ((uint8_t)0, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)0, short_buffer.Size());

    for (size_t i = 0; i < 128; ++i) {
        pos = 0;
#define TEST_PUSH_BACK_HELPER(type) ASSERT_TRUE(short_buffer.PushBack<type>(pos++, i));

        NUMBER_TYPE_HELPER_FOR_TEST(TEST_PUSH_BACK_HELPER);
        short_buffer.EndPushBack();
    }

    ASSERT_EQ((uint8_t)128, short_buffer.Capacity());
    ASSERT_EQ((uint8_t)128, short_buffer.Size());

    for (size_t i = 0; i < 128; ++i) {
        pos = 0;
#define ASSERT_GET_VALUE_HELPER(type) ASSERT_EQ((type)(i), short_buffer.GetRowTyped<type>(pos++)[i]);

        NUMBER_TYPE_HELPER_FOR_TEST(ASSERT_GET_VALUE_HELPER);
    }
}

} // namespace infinity