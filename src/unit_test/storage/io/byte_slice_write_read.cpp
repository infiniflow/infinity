#include "unit_test/base_test.h"
#include <iostream>

import stl;

import file_writer;
import file_reader;
import local_file_system;
import byte_slice;
import byte_slice_reader;
import byte_slice_writer;

using namespace infinity;

class ByteSliceReaderWriterTest : public BaseTest {
public:
    template <typename T>
    void CheckList(const ByteSliceList *list, u32 num_elem, T value = 0) {
        ASSERT_EQ(num_elem * sizeof(T), list->GetTotalSize());
        u8 *buffer = new u8[list->GetTotalSize()];
        size_t n = 0;
        ByteSlice *slice = list->GetHead();
        while (slice) {
            memcpy(buffer + n, slice->data_, slice->size_);
            n += slice->size_;
            ASSERT_TRUE(n <= list->GetTotalSize());
            slice = slice->next_;
        }

        n = 0;
        u32 i;
        for (i = 0; i < num_elem; i++) {
            if (value == (T)0) {
                ASSERT_EQ((T)i, *(T *)(buffer + n));
            } else {
                ASSERT_EQ((T)value, *(T *)(buffer + n));
            }
            n += sizeof(T);
        }
        ASSERT_EQ(i, num_elem);
        delete[] buffer;
    }
};

TEST_F(ByteSliceReaderWriterTest, test1) {
    using namespace infinity;
    ByteSliceWriter writer;
    u32 size = 0;

    u8 byte_value = 12;
    writer.WriteByte(byte_value);
    size += sizeof(byte_value);
    ASSERT_EQ(size, writer.GetSize());

    i16 i16_value = 120;
    writer.WriteInt16(i16_value);
    size += sizeof(i16);
    ASSERT_EQ(size, writer.GetSize());

    i32 i32_value = 12;
    writer.WriteInt32(i32_value);
    size += sizeof(i32);
    ASSERT_EQ(size, writer.GetSize());

    u32 u32_value = 21;
    writer.WriteUInt32(u32_value);
    size += sizeof(u32);
    ASSERT_EQ(size, writer.GetSize());
}

TEST_F(ByteSliceReaderWriterTest, test2) {
    using namespace infinity;
    ByteSliceWriter writer;
    i16 i = 0;
    for (i = 0; i < 10000; i++) {
        writer.WriteInt16(i);
    }
    ASSERT_EQ(i * sizeof(i16), writer.GetSize());
    CheckList<i16>(writer.GetByteSliceList(), i);
}

TEST_F(ByteSliceReaderWriterTest, test3) {
    using namespace infinity;
    ByteSliceWriter writer;
    String str(16000, 'x');
    writer.Write(str.c_str(), str.size());
    ASSERT_EQ(str.size(), writer.GetSize());
    CheckList<char>(writer.GetByteSliceList(), 16000, 'x');
}

TEST_F(ByteSliceReaderWriterTest, test4) {
    using namespace infinity;
    ByteSliceWriter writer;
    i32 i = 0;
    for (i = 0; i < 10000; i++) {
        writer.WriteVInt(i);
    }

    ByteSliceReader reader(writer.GetByteSliceList());
    for (i = 0; i < 10000; i++) {
        i32 value = reader.ReadVInt32();
        ASSERT_EQ(value, i);
    }
}

TEST_F(ByteSliceReaderWriterTest, test5) {
    using namespace infinity;
    ByteSliceWriter writer;
    i64 i = 0;
    for (i = 0; i < 10000; i++) {
        writer.WriteVLong(i);
    }

    ByteSliceReader reader(writer.GetByteSliceList());
    for (i = 0; i < 10000; i++) {
        i64 value = reader.ReadVUInt64();
        ASSERT_EQ(value, i);
    }
}
