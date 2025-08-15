
#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.posting_buffer;

import :ut.base_test;

import :index_defines;
import :posting_field;
import :posting_buffer;
#endif

using namespace infinity;

namespace infinity {

template <typename T>
class SimpleFormat : public PostingFields {
public:
    SimpleFormat(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            PostingField *value = new TypedPostingField<T>();
            value->location_ = i;
            value->offset_ = i * sizeof(T);
            values_.push_back(value);
        }
    }
};
} // namespace infinity

class PostingBufferTest : public BaseTest {

protected:
    void
    CheckPostingBuffer(u32 *expect_posting_buffer, size_t row_num, size_t col_num, PostingBuffer *posting_buffer = nullptr, u8 *capacity = nullptr);

    void CheckEqual(u32 *expect_posting_buffer, size_t row_num, size_t col_num, PostingBuffer *posting_buffer);
};

void PostingBufferTest::CheckPostingBuffer(u32 *expect_posting_buffer, size_t row_num, size_t col_num, PostingBuffer *posting_buffer, u8 *capacity) {
    using namespace infinity;
    std::unique_ptr<PostingBuffer> posting_buffer_ptr;
    SimpleFormat<u32> posting_fields(row_num);
    if (!posting_buffer) {
        posting_buffer = new PostingBuffer;
        posting_buffer_ptr.reset(posting_buffer);
        posting_buffer_ptr->Init(&posting_fields);
    }
    u8 cur_capacity = capacity == nullptr ? 0 : *capacity;
    ASSERT_EQ(cur_capacity, posting_buffer->Capacity());
    ASSERT_EQ((u8)0, posting_buffer->Size());
    ASSERT_EQ(row_num, posting_buffer->GetRowCount());

    for (size_t c = 0; c < col_num; ++c) {
        for (size_t r = 0; r < row_num; ++r) {
            ASSERT_TRUE(posting_buffer->PushBack(r, expect_posting_buffer[c + r * col_num]));
        }
        posting_buffer->EndPushBack();
        ASSERT_EQ((u8)(c + 1), posting_buffer->Size());
        u8 cur_capacity = capacity == nullptr ? posting_buffer->AllocatePlan(c) : *capacity;
        ASSERT_EQ(cur_capacity, posting_buffer->Capacity());
    }

    CheckEqual(expect_posting_buffer, row_num, col_num, posting_buffer);
}

void PostingBufferTest::CheckEqual(u32 *expect_posting_buffer, size_t row_num, size_t col_num, PostingBuffer *posting_buffer) {
    using namespace infinity;
    ASSERT_EQ(row_num, (size_t)posting_buffer->GetRowCount());
    ASSERT_EQ(col_num, (size_t)posting_buffer->Size());
    for (size_t r = 0; r < row_num; ++r) {
        u32 *row = posting_buffer->GetRowTyped<u32>(r);
        for (size_t c = 0; c < col_num; ++c) {
            ASSERT_EQ(expect_posting_buffer[c + r * col_num], row[c]);
            u32 *row = posting_buffer->GetRowTyped<u32>(r);
            ASSERT_EQ(expect_posting_buffer[c + r * col_num], row[c]);
        }
    }
}

TEST_F(PostingBufferTest, test1) {
    using namespace infinity;
    SimpleFormat<u32> posting_fields(1);
    PostingBuffer posting_buffer;
    posting_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, posting_buffer.Capacity());
    ASSERT_EQ((u8)0, posting_buffer.Size());

    ASSERT_TRUE(posting_buffer.PushBack(0, (u32)100));
    posting_buffer.EndPushBack();
    ASSERT_EQ((u8)2, posting_buffer.Capacity());
    ASSERT_EQ((u8)1, posting_buffer.Size());
}

TEST_F(PostingBufferTest, test2) {
    using namespace infinity;
    u32 expect_posting_buffer[] = {100, 101, 102, 200, 201, 202};
    CheckPostingBuffer((u32 *)expect_posting_buffer, 2, 3);
}

TEST_F(PostingBufferTest, test3) {
    using namespace infinity;
    PostingBuffer posting_buffer;
    SimpleFormat<u32> posting_fields(8);
    posting_buffer.Init(&posting_fields);

    u32 expect_posting_buffer[128 * 8];
    for (size_t c = 0; c < 128; ++c) {
        for (size_t r = 0; r < 8; ++r) {
            expect_posting_buffer[c + r * 128] = random();
        }
    }
    CheckPostingBuffer((u32 *)expect_posting_buffer, 8, 128, &posting_buffer);

    posting_buffer.Clear();

    u8 capcity = 128;
    CheckPostingBuffer((u32 *)expect_posting_buffer, 8, 128, &posting_buffer, &capcity);
}

TEST_F(PostingBufferTest, test4) {
    using namespace infinity;
    {
        // push same row repeatedly
        PostingBuffer posting_buffer;
        SimpleFormat<u32> posting_fields(2);
        posting_buffer.Init(&posting_fields);

        ASSERT_TRUE(posting_buffer.PushBack<u32>(0, 100));
        posting_buffer.EndPushBack();
        ASSERT_TRUE(posting_buffer.PushBack<u32>(0, 200));
    }
    {
        // push when posting_buffer is full and hit max capacity
        PostingBuffer posting_buffer;
        SimpleFormat<u32> posting_fields(1);
        posting_buffer.Init(&posting_fields);

        for (size_t col = 0; col < 128; ++col) {
            ASSERT_TRUE(posting_buffer.PushBack<u32>(0, col));
            posting_buffer.EndPushBack();
        }
        ASSERT_TRUE(!posting_buffer.PushBack<u32>(0, 128));
    }
}

TEST_F(PostingBufferTest, test5) {
    using namespace infinity;
    PostingBuffer posting_buffer;
    SimpleFormat<u32> posting_fields(2);
    posting_buffer.Init(&posting_fields);

    ASSERT_TRUE(posting_buffer.PushBack<u32>(0, 100));
    ASSERT_EQ((u8)0, posting_buffer.Size());

    ASSERT_TRUE(posting_buffer.PushBack<u32>(1, 200));
    posting_buffer.EndPushBack();
    ASSERT_EQ((u8)1, posting_buffer.Size());
}

TEST_F(PostingBufferTest, test6) {
    using namespace infinity;
    PostingBuffer posting_buffer;
    SimpleFormat<u32> posting_fields(3);
    posting_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, posting_buffer.Capacity());
    ASSERT_EQ((u8)0, posting_buffer.Size());

    ASSERT_TRUE(posting_buffer.PushBack<u32>(2, 100));

    ASSERT_EQ((u8)2, posting_buffer.Capacity());
    ASSERT_EQ((u8)0, posting_buffer.Size());

    ASSERT_TRUE(posting_buffer.PushBack<u32>(0, 100));

    ASSERT_EQ((u8)2, posting_buffer.Capacity());
    ASSERT_EQ((u8)0, posting_buffer.Size());

    ASSERT_TRUE(posting_buffer.PushBack<u32>(1, 100));
    posting_buffer.EndPushBack();
    ASSERT_EQ((u8)2, posting_buffer.Capacity());
    ASSERT_EQ((u8)1, posting_buffer.Size());
}

TEST_F(PostingBufferTest, test7) {
    using namespace infinity;
    PostingBuffer posting_buffer;
    SimpleFormat<u32> posting_fields(1);
    posting_buffer.Init(&posting_fields);

    posting_buffer.PushBack<u32>(0, 100);
    posting_buffer.EndPushBack();
    ASSERT_EQ(1, posting_buffer.Size());
    ASSERT_EQ(2, posting_buffer.Capacity());

    posting_buffer.Clear();
    ASSERT_EQ(0, posting_buffer.Size());
    ASSERT_EQ(2, posting_buffer.Capacity());
}

TEST_F(PostingBufferTest, test8) {
    using namespace infinity;

#define NUMBER_TYPE_HELPER_FOR_TEST(macro)                                                                                                           \
    macro(u32);                                                                                                                                      \
    macro(u32);                                                                                                                                      \
    macro(uint16_t);                                                                                                                                 \
    macro(uint16_t);

    PostingFields posting_fields;
    size_t offset = 0;
    size_t pos = 0;

#define ADD_VALUE_HELPER_FOR_TEST(type)                                                                                                              \
    {                                                                                                                                                \
        PostingField *value = new TypedPostingField<type>();                                                                                         \
        value->location_ = pos++;                                                                                                                    \
        value->offset_ = offset;                                                                                                                     \
        offset += sizeof(type);                                                                                                                      \
        posting_fields.AddValue(value);                                                                                                              \
    }

    NUMBER_TYPE_HELPER_FOR_TEST(ADD_VALUE_HELPER_FOR_TEST);

    PostingBuffer posting_buffer;
    posting_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, posting_buffer.Capacity());
    ASSERT_EQ((u8)0, posting_buffer.Size());

    for (size_t i = 0; i < 128; ++i) {
        pos = 0;
#define TEST_PUSH_BACK_HELPER(type) ASSERT_TRUE(posting_buffer.PushBack<type>(pos++, i));

        NUMBER_TYPE_HELPER_FOR_TEST(TEST_PUSH_BACK_HELPER);
        posting_buffer.EndPushBack();
    }

    ASSERT_EQ((u8)128, posting_buffer.Capacity());
    ASSERT_EQ((u8)128, posting_buffer.Size());

    for (size_t i = 0; i < 128; ++i) {
        pos = 0;
#define ASSERT_GET_VALUE_HELPER(type) ASSERT_EQ((type)(i), posting_buffer.GetRowTyped<type>(pos++)[i]);

        NUMBER_TYPE_HELPER_FOR_TEST(ASSERT_GET_VALUE_HELPER);
    }
}
