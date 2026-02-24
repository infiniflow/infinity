module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.byte_slice_write_read;

import :ut.base_test;
import :file_writer;
import :file_reader;
import :byte_slice;
import :byte_slice_reader;
import :byte_slice_writer;
import :logger;

import third_party;

import global_resource_usage;

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

    u8 *GetData(const ByteSliceList *list) {
        u8 *buffer = new u8[list->GetTotalSize()];
        size_t n = 0;
        ByteSlice *slice = list->GetHead();
        while (slice) {
            memcpy(buffer + n, slice->data_, slice->size_);
            n += slice->size_;
            // std::cout << "n: " << n << ", list->GetTotalSize(): " << list->GetTotalSize() << std::endl;
            // ASSERT_TRUE((n <= list->GetTotalSize()));
            slice = slice->next_;
        }
        return buffer;
    }

    bool CheckListEq(const ByteSliceList *list_1, const ByteSliceList *list_2) {
        if (list_1->GetTotalSize() != list_2->GetTotalSize()) {
            return false;
        }
        auto buffer_1 = GetData(list_1);
        auto buffer_2 = GetData(list_2);

        bool eq = memcmp(buffer_1, buffer_2, list_1->GetTotalSize()) == 0;

        delete[] buffer_1;
        delete[] buffer_2;
        return eq;
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
    std::string str(16000, 'x');
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

TEST_F(ByteSliceReaderWriterTest, test6) {
    using namespace infinity;
    std::string path = "/var/infinity/tmp/test001.txt";

    // Prepare data
    ByteSliceWriter writer;
    i16 i;
    for (i = 0; i < 1000; i++) {
        writer.WriteInt16(i);
    }
    size_t size = writer.GetSize();
    ASSERT_EQ(i * sizeof(i16), size);

    // Dump data
    auto file_writer = std::make_shared<FileWriter>(path, 128000);
    writer.Dump(file_writer);
    file_writer->Sync();

    writer.Reset();

    // Load data
    auto file_reader = std::make_shared<FileReader>(path, 128000);
    writer.Load(file_reader, 1000 * sizeof(i16));

    // Verify the loaded data
    ByteSliceReader reader(writer.GetByteSliceList());
    for (i = 0; i < 1000; i++) {
        i16 value = reader.ReadInt16();
        ASSERT_EQ(value, i);
    }
}

TEST_F(ByteSliceReaderWriterTest, test7) {
    using namespace infinity;

    // Prepare data
    ByteSliceWriter writer1;
    i16 i;
    for (i = 500; i < 1000; i++) {
        writer1.WriteInt16(i);
    }
    ASSERT_EQ(writer1.GetSize(), 500 * sizeof(i16));

    ByteSliceWriter writer2;
    for (i = 1000; i < 1500; i++) {
        writer2.WriteInt16(i);
    }
    ASSERT_EQ(writer2.GetSize(), 500 * sizeof(i16));

    // Merge data (head --> writer2's data --> writer1's data --> tail)
    writer2.Write(*writer1.GetByteSliceList());

    ASSERT_EQ(writer1.GetSize(), 0);
    ASSERT_EQ(writer2.GetSize(), 1000 * sizeof(i16));

    // Verify data
    ByteSliceReader reader1(writer2.GetByteSliceList());
    for (i = 1000; i < 1500; i++) {
        i16 value = reader1.ReadInt16();
        ASSERT_EQ(value, i);
    }
    for (i = 500; i < 1000; i++) {
        i16 value = reader1.ReadInt16();
        ASSERT_EQ(value, i);
    }

    // Prepare data
    ByteSliceWriter writer3;
    for (i = 1500; i < 2000; i++) {
        writer3.WriteInt16(i);
    }
    ASSERT_EQ(writer3.GetSize(), 500 * sizeof(i16));

    // Merge data (head --> writer3's data --> writer2's partial data --> tail)
    writer3.Write(*writer2.GetByteSliceList(), 0, writer2.GetSize() / 2 + 100);

    // There must be something wrong
    // ASSERT_EQ(writer2.GetSize(), 0);
    // ASSERT_EQ(writer3.GetSize(), (500 + 500 + 50) * sizeof(i16));

    // Verify data
    ByteSliceReader reader2(writer3.GetByteSliceList());
    for (i = 1500; i < 2000; i++) {
        i16 value = reader2.ReadInt16();
        ASSERT_EQ(value, i);
    }
    for (i = 1000; i < 1500; i++) {
        i16 value = reader2.ReadInt16();
        ASSERT_EQ(value, i);
    }
    for (i = 500; i < 550; i++) {
        i16 value = reader2.ReadInt16();
        ASSERT_EQ(value, i);
    }

    auto slice_list = writer3.GetByteSliceList();
    size_t total_size = slice_list->UpdateTotalSize();
    ASSERT_EQ(total_size, (500 + 500 + 50) * sizeof(i16));

    auto slice_iter = ByteSliceListIterator(slice_list);
    slice_iter.SeekSlice(0);

    void *data;
    size_t size;
    while (slice_iter.HasNext(writer3.GetSize())) {
        slice_iter.Next(data, size);
        LOG_INFO(fmt::format("size: {}", size));
    }
}

TEST_F(ByteSliceReaderWriterTest, TestDataConsistency) {
    using namespace infinity;
    ByteSliceWriter writer;

    // Write various data
    writer.WriteByte(255);
    writer.WriteInt16(-12345);
    writer.WriteInt32(123456789);
    writer.WriteUInt32(987654321);

    ByteSliceReader reader(writer.GetByteSliceList());

    // Verify data consistency
    ASSERT_EQ(reader.ReadByte(), 255);
    ASSERT_EQ(reader.ReadInt16(), -12345);
    ASSERT_EQ(reader.ReadInt32(), 123456789);
    ASSERT_EQ(reader.ReadUInt32(), 987654321);
}
