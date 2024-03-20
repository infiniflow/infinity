
#include "unit_test/base_test.h"

import stl;
import memory_pool;
import index_defines;
import posting_field;
import short_buffer;

namespace infinity {

template <typename T>
class SimpleFormat : public PostingFields {
public:
    SimpleFormat(SizeT count) {
        for (SizeT i = 0; i < count; ++i) {
            PostingField *value = new TypedPostingField<T>();
            value->location_ = i;
            value->offset_ = i * sizeof(T);
            values_.push_back(value);
        }
    }
};
} // namespace infinity

using namespace infinity;

class ShortBufferTest : public BaseTest {

protected:
    void CheckShortBuffer(u32 *expect_short_buffer, SizeT row_num, SizeT col_num, ShortBuffer *short_buffer = nullptr, u8 *capacity = nullptr);

    void CheckEqual(u32 *expect_short_buffer, SizeT row_num, SizeT col_num, ShortBuffer *short_buffer);
};

void ShortBufferTest::CheckShortBuffer(u32 *expect_short_buffer, SizeT row_num, SizeT col_num, ShortBuffer *short_buffer, u8 *capacity) {
    using namespace infinity;
    UniquePtr<ShortBuffer> short_buffer_ptr;
    SimpleFormat<u32> posting_fields(row_num);
    if (!short_buffer) {
        short_buffer = new ShortBuffer;
        short_buffer_ptr.reset(short_buffer);
        short_buffer_ptr->Init(&posting_fields);
    }
    u8 cur_capacity = capacity == nullptr ? 0 : *capacity;
    ASSERT_EQ(cur_capacity, short_buffer->Capacity());
    ASSERT_EQ((u8)0, short_buffer->Size());
    ASSERT_EQ(row_num, short_buffer->GetRowCount());

    for (SizeT c = 0; c < col_num; ++c) {
        for (SizeT r = 0; r < row_num; ++r) {
            ASSERT_TRUE(short_buffer->PushBack(r, expect_short_buffer[c + r * col_num]));
        }
        short_buffer->EndPushBack();
        ASSERT_EQ((u8)(c + 1), short_buffer->Size());
        u8 cur_capacity = capacity == nullptr ? short_buffer->AllocatePlan(c) : *capacity;
        ASSERT_EQ(cur_capacity, short_buffer->Capacity());
    }

    CheckEqual(expect_short_buffer, row_num, col_num, short_buffer);
}

void ShortBufferTest::CheckEqual(u32 *expect_short_buffer, SizeT row_num, SizeT col_num, ShortBuffer *short_buffer) {
    using namespace infinity;
    ASSERT_EQ(row_num, (SizeT)short_buffer->GetRowCount());
    ASSERT_EQ(col_num, (SizeT)short_buffer->Size());
    for (SizeT r = 0; r < row_num; ++r) {
        u32 *row = short_buffer->GetRowTyped<u32>(r);
        for (SizeT c = 0; c < col_num; ++c) {
            ASSERT_EQ(expect_short_buffer[c + r * col_num], row[c]);
            u32 *row = short_buffer->GetRowTyped<u32>(r);
            ASSERT_EQ(expect_short_buffer[c + r * col_num], row[c]);
        }
    }
}

TEST_F(ShortBufferTest, test1) {
    using namespace infinity;
    SimpleFormat<u32> posting_fields(1);
    ShortBuffer short_buffer;
    short_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, short_buffer.Capacity());
    ASSERT_EQ((u8)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack(0, (u32)100));
    short_buffer.EndPushBack();
    ASSERT_EQ((u8)2, short_buffer.Capacity());
    ASSERT_EQ((u8)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test2) {
    using namespace infinity;
    u32 expect_short_buffer[] = {100, 101, 102, 200, 201, 202};
    CheckShortBuffer((u32 *)expect_short_buffer, 2, 3);
}

TEST_F(ShortBufferTest, test3) {
    using namespace infinity;
    ShortBuffer short_buffer;
    SimpleFormat<u32> posting_fields(8);
    short_buffer.Init(&posting_fields);

    u32 expect_short_buffer[128 * 8];
    for (SizeT c = 0; c < 128; ++c) {
        for (SizeT r = 0; r < 8; ++r) {
            expect_short_buffer[c + r * 128] = random();
        }
    }
    CheckShortBuffer((u32 *)expect_short_buffer, 8, 128, &short_buffer);

    short_buffer.Clear();

    u8 capcity = 128;
    CheckShortBuffer((u32 *)expect_short_buffer, 8, 128, &short_buffer, &capcity);
}

TEST_F(ShortBufferTest, test4) {
    using namespace infinity;
    {
        // push same row repeatedly
        ShortBuffer short_buffer;
        SimpleFormat<u32> posting_fields(2);
        short_buffer.Init(&posting_fields);

        ASSERT_TRUE(short_buffer.PushBack<u32>(0, 100));
        short_buffer.EndPushBack();
        ASSERT_TRUE(short_buffer.PushBack<u32>(0, 200));
    }
    {
        // push when short_buffer is full and hit max capacity
        ShortBuffer short_buffer;
        SimpleFormat<u32> posting_fields(1);
        short_buffer.Init(&posting_fields);

        for (SizeT col = 0; col < 128; ++col) {
            ASSERT_TRUE(short_buffer.PushBack<u32>(0, col));
            short_buffer.EndPushBack();
        }
        ASSERT_TRUE(!short_buffer.PushBack<u32>(0, 128));
    }
}

TEST_F(ShortBufferTest, test5) {
    using namespace infinity;
    ShortBuffer short_buffer;
    SimpleFormat<u32> posting_fields(2);
    short_buffer.Init(&posting_fields);

    ASSERT_TRUE(short_buffer.PushBack<u32>(0, 100));
    ASSERT_EQ((u8)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<u32>(1, 200));
    short_buffer.EndPushBack();
    ASSERT_EQ((u8)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test6) {
    using namespace infinity;
    ShortBuffer short_buffer;
    SimpleFormat<u32> posting_fields(3);
    short_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, short_buffer.Capacity());
    ASSERT_EQ((u8)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<u32>(2, 100));

    ASSERT_EQ((u8)2, short_buffer.Capacity());
    ASSERT_EQ((u8)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<u32>(0, 100));

    ASSERT_EQ((u8)2, short_buffer.Capacity());
    ASSERT_EQ((u8)0, short_buffer.Size());

    ASSERT_TRUE(short_buffer.PushBack<u32>(1, 100));
    short_buffer.EndPushBack();
    ASSERT_EQ((u8)2, short_buffer.Capacity());
    ASSERT_EQ((u8)1, short_buffer.Size());
}

TEST_F(ShortBufferTest, test7) {
    using namespace infinity;
    ShortBuffer short_buffer;
    SimpleFormat<u32> posting_fields(1);
    short_buffer.Init(&posting_fields);

    short_buffer.PushBack<u32>(0, 100);
    short_buffer.EndPushBack();
    ASSERT_EQ(1, short_buffer.Size());
    ASSERT_EQ(2, short_buffer.Capacity());

    short_buffer.Clear();
    ASSERT_EQ(0, short_buffer.Size());
    ASSERT_EQ(2, short_buffer.Capacity());
}

TEST_F(ShortBufferTest, test8) {
    using namespace infinity;

// TODO some type order might fail when using memory pool
#define NUMBER_TYPE_HELPER_FOR_TEST(macro)                                                                                                           \
    macro(u32);                                                                                                                                      \
    macro(u32);                                                                                                                                      \
    macro(uint16_t);                                                                                                                                 \
    macro(uint16_t);

    PostingFields posting_fields;
    SizeT offset = 0;
    SizeT pos = 0;

#define ADD_VALUE_HELPER_FOR_TEST(type)                                                                                                              \
    {                                                                                                                                                \
        PostingField *value = new TypedPostingField<type>();                                                                                         \
        value->location_ = pos++;                                                                                                                    \
        value->offset_ = offset;                                                                                                                     \
        offset += sizeof(type);                                                                                                                      \
        posting_fields.AddValue(value);                                                                                                              \
    }

    NUMBER_TYPE_HELPER_FOR_TEST(ADD_VALUE_HELPER_FOR_TEST);

    ShortBuffer short_buffer;
    short_buffer.Init(&posting_fields);

    ASSERT_EQ((u8)0, short_buffer.Capacity());
    ASSERT_EQ((u8)0, short_buffer.Size());

    for (SizeT i = 0; i < 128; ++i) {
        pos = 0;
#define TEST_PUSH_BACK_HELPER(type) ASSERT_TRUE(short_buffer.PushBack<type>(pos++, i));

        NUMBER_TYPE_HELPER_FOR_TEST(TEST_PUSH_BACK_HELPER);
        short_buffer.EndPushBack();
    }

    ASSERT_EQ((u8)128, short_buffer.Capacity());
    ASSERT_EQ((u8)128, short_buffer.Size());

    for (SizeT i = 0; i < 128; ++i) {
        pos = 0;
#define ASSERT_GET_VALUE_HELPER(type) ASSERT_EQ((type)(i), short_buffer.GetRowTyped<type>(pos++)[i]);

        NUMBER_TYPE_HELPER_FOR_TEST(ASSERT_GET_VALUE_HELPER);
    }
}
