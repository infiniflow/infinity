#include "unit_test/base_test.h"

import stl;
import memory_pool;
import buffered_skiplist_writer;
import buffered_byte_slice_reader;
import index_defines;
import posting_field;
import file_writer;
import file_system;
import local_file_system;

using namespace infinity;

class BufferedSkipListWriterTest : public BaseTest {
public:
    BufferedSkipListWriterTest() {
        byte_slice_pool_ = new MemoryPool(BUFFER_SIZE_);
        buffer_pool_ = new RecyclePool(BUFFER_SIZE_);
//        void *buffer = byte_slice_pool_->Allocate(sizeof(BufferedSkipListWriter));
//        buffered_skiplist_writer_.reset(new (buffer) BufferedSkipListWriter(byte_slice_pool_, buffer_pool_));
        buffered_skiplist_writer_ = MakeShared<BufferedSkipListWriter>(byte_slice_pool_, buffer_pool_);
    }
    ~BufferedSkipListWriterTest() {
        delete byte_slice_pool_;
        delete buffer_pool_;
    }
    void SetUp() override {}

    void TearDown() override { buffered_skiplist_writer_.reset(); }

protected:
    SharedPtr<FileWriter> CreateFileWriter(String file_name);

protected:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    SharedPtr<BufferedSkipListWriter> buffered_skiplist_writer_ = nullptr;
    const SizeT BUFFER_SIZE_ = 1024;
};

SharedPtr<FileWriter> BufferedSkipListWriterTest::CreateFileWriter(infinity::String file_path) {
    auto fs = MakeShared<LocalFileSystem>();
    return MakeShared<FileWriter>(*fs, file_path, BUFFER_SIZE_);
}

TEST_F(BufferedSkipListWriterTest, test1) {
    using namespace infinity;

    ASSERT_NE(nullptr, buffered_skiplist_writer_);
    PostingFields posting_fields;

    // auto posting_field = MakeShared<TypedPostingField<u32>>();
    auto posting_field = new TypedPostingField<u32>();
    posting_fields.AddValue(posting_field);

    //    std::cout << "values size: " << posting_fields.values_.size() << std::endl;
    //    std::cout << "values[0] size: " << posting_fields.values_[0]->GetSize() << std::endl;
    //
    //    std::cout << posting_fields.GetTotalSize() << std::endl;

    buffered_skiplist_writer_->Init(&posting_fields);
    const u32 delta = 10;
    // auto short_buffer = buffered_skiplist_writer_->GetBuffer();
    // std::cout << "capacity: " << static_cast<int>(short_buffer.Capacity()) << std::endl;
    // std::cout << "size: " << static_cast<int>(short_buffer.Size()) << std::endl;
    // std::cout << "new capacity = " << static_cast<int>(short_buffer.AllocatePlan(short_buffer.Capacity())) << std::endl;

    // ASSERT_NE(short_buffer.GetPostingValues(), nullptr);

    // short_buffer.PushBack(0, 1);
    // short_buffer.

    // buffered_skiplist_writer_->AddItem(delta);


    //    auto fs = MakeShared<LocalFileSystem>();
    //    String file_path = "buffered_skiplist_writer_test1.txt";
    //    const SizeT BUFFER_SIZE = 1024;
    //    auto file_writer = MakeShared<FileWriter>(*fs, file_path, BUFFER_SIZE);
    String file_path = "buffered_skiplist_writer_test1.txt";
    auto file_writer = CreateFileWriter(file_path);
    //    std::cout << "SKIP_LIST_BUFFER_SIZE = " << static_cast<int>(SKIP_LIST_BUFFER_SIZE) << std::endl;
    //
    //    std::cout << "EstimateDumpSize: " << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;
    //    std::cout << "sizeof u32 = " << sizeof(u32) << std::endl;

    Vector<u32> expected_values;
    u32 now_value = 0;
    for (int i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        buffered_skiplist_writer_->AddItem(delta);
        now_value += delta;
        expected_values.emplace_back(now_value);
        // std::cout << "after insert " << i << ", EstimateDumpSize=" << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;
    }

    // SizeT expected_dump_size = 40;
    // ASSERT_EQ(expected_dump_size, buffered_skiplist_writer_->EstimateDumpSize());

    // buffered_skiplist_writer_->Dump(file_writer);

    // std::cout << "Before Dump EstimateDumpSize: " << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;
    buffered_skiplist_writer_->Dump(file_writer);
    // std::cout << "After Dump EstimateDumpSize: " << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;

    // SharedPtr<BufferedByteSliceReader> reader = MakeShared<BufferedByteSliceReader>();
    auto reader = MakeShared<BufferedByteSliceReader>();
    reader->Open(buffered_skiplist_writer_.get());

    u32 val_buffer[BUFFER_SIZE_];
    SizeT decode_len;
    ASSERT_TRUE(reader->Decode<u32>(val_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode<u32>(val_buffer + SKIP_LIST_BUFFER_SIZE, 2, decode_len));

    //    std::cout << "decode count: " << decode_len << std::endl;
    //    std::cout << static_cast<int>(pos_buffer[0]) << std::endl;
    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        auto value = val_buffer[i];
        // std::cout << i << " " << value << " : " << expected_values[i] << std::endl;
        ASSERT_EQ(value, expected_values[i]);
    }
    //    for (size_t i = SKIP_LIST_BUFFER_SIZE; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
    //        auto value = val_buffer[i];
    //        ASSERT_EQ(value, expected_values[i]);
    //    }
    //    std::cout << val_buffer[0] << " " << val_buffer[1] << std::endl;

    // TODO yzc:
    // invokes AddItem(u32 key) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
    //    std::cout << "end" << std::endl;
}

TEST_F(BufferedSkipListWriterTest, test2) {
    using namespace infinity;

    ASSERT_NE(nullptr, buffered_skiplist_writer_);
    PostingFields posting_fields;

    auto posting_field_key = new TypedPostingField<u32>();
    auto posting_field_value = new TypedPostingField<u32>();
    posting_fields.AddValue(posting_field_key);
    posting_fields.AddValue(posting_field_value);

    buffered_skiplist_writer_->Init(&posting_fields);
    const u32 delta = 10;

    String file_path = "buffered_skiplist_writer_test2.txt";
    auto file_writer = CreateFileWriter(file_path);

    Vector<u32> expected_keys;
    Vector<u32> expected_values;

    u32 now_key = 0;
    u32 now_value = 0;
    for (int i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        now_key += delta;
        now_value += delta + 1;
        buffered_skiplist_writer_->AddItem(now_key, now_value);
        expected_keys.emplace_back(now_key);
        expected_values.emplace_back(now_value);
        // std::cout << "after insert " << i << ", EstimateDumpSize=" << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;
    }
    // SizeT expected_dump_size = 40;
    // ASSERT_EQ(expected_dump_size, buffered_skiplist_writer_->EstimateDumpSize());

    // buffered_skiplist_writer_->Dump(file_writer);

    // std::cout << "Before Dump EstimateDumpSize: " << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;
    buffered_skiplist_writer_->Dump(file_writer);
    // std::cout << "After Dump EstimateDumpSize: " << buffered_skiplist_writer_->EstimateDumpSize() << std::endl;

    // SharedPtr<BufferedByteSliceReader> reader = MakeShared<BufferedByteSliceReader>();
    auto reader = MakeShared<BufferedByteSliceReader>();
    reader->Open(buffered_skiplist_writer_.get());

    u32 val_buffer[BUFFER_SIZE_];
    SizeT decode_len;

    reader->Seek(0);
    ASSERT_TRUE(reader->Decode<u32>(val_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode<u32>(val_buffer + SKIP_LIST_BUFFER_SIZE, 2, decode_len));

    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        std::cout << i << " " << val_buffer[i] << std::endl;
    }

    reader->Seek(1);
    ASSERT_TRUE(reader->Decode<u32>(val_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode<u32>(val_buffer + SKIP_LIST_BUFFER_SIZE, 2, decode_len));

    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        std::cout << i << " " << val_buffer[i] << std::endl;
    }

    //    std::cout << "decode count: " << decode_len << std::endl;
    //    std::cout << static_cast<int>(pos_buffer[0]) << std::endl;
    //    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
    //        auto value = val_buffer[i];
    //        // std::cout << i << " " << value << " : " << expected_values[i] << std::endl;
    //        ASSERT_EQ(value, expected_values[i]);
    //    }

    // TODO yzc:
    // invokes AddItem(u32 key, u32 value1) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
}

TEST_F(BufferedSkipListWriterTest, test3) {
    using namespace infinity;

    // TODO yzc:
    // invokes AddItem(u32 key, u32 value1, u32 value2) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
}
