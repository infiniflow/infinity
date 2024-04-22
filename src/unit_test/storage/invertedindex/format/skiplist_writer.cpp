#include "unit_test/base_test.h"

import stl;
import memory_pool;
import skiplist_writer;
import posting_byte_slice_reader;
import posting_byte_slice;
import index_defines;
import posting_field;
import file_writer;
import file_system;
import local_file_system;

using namespace infinity;

class SkipListWriterTest : public BaseTest {
public:
    SkipListWriterTest() {
        byte_slice_pool_ = new MemoryPool(BUFFER_SIZE_);
        buffer_pool_ = new RecyclePool(BUFFER_SIZE_);
    }
    ~SkipListWriterTest() {
        delete byte_slice_pool_;
        delete buffer_pool_;
    }
    void SetUp() override { skiplist_writer_ = MakeShared<SkipListWriter>(byte_slice_pool_, buffer_pool_); }

    void TearDown() override { skiplist_writer_.reset(); }

protected:
    SharedPtr<FileWriter> CreateFileWriter(String file_name);

protected:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    SharedPtr<SkipListWriter> skiplist_writer_ = nullptr;
    static constexpr SizeT BUFFER_SIZE_ = 1024;
};

SharedPtr<FileWriter> SkipListWriterTest::CreateFileWriter(infinity::String file_path) {
    auto fs = MakeShared<LocalFileSystem>();
    return MakeShared<FileWriter>(*fs, file_path, BUFFER_SIZE_);
}

TEST_F(SkipListWriterTest, test1) {
    using namespace infinity;

    ASSERT_NE(nullptr, skiplist_writer_);
    PostingFields posting_fields;
    auto posting_field = new TypedPostingField<u32>();
    posting_field->encoder_ = GetSkipListEncoder();
    posting_fields.AddValue(posting_field);
    skiplist_writer_->Init(&posting_fields);
    const u32 delta = 10;

    String file_path = String(GetTmpDir()) + "/skiplist_writer_test1.txt";
    auto file_writer = CreateFileWriter(file_path);

    Vector<u32> expected_values;
    u32 now_value = 0;
    for (int i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        skiplist_writer_->AddItem(delta);
        now_value += delta;
        expected_values.emplace_back(now_value);
    }

    skiplist_writer_->Dump(file_writer);

    auto reader = MakeShared<PostingByteSliceReader>();
    reader->Open(skiplist_writer_.get());

    u32 val_buffer[BUFFER_SIZE_];
    SizeT decode_len;
    ASSERT_TRUE(reader->Decode<u32>(val_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode<u32>(val_buffer + SKIP_LIST_BUFFER_SIZE, 2, decode_len));

    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        auto value = val_buffer[i];
        ASSERT_EQ(value, expected_values[i]);
    }
}

TEST_F(SkipListWriterTest, test2) {
    using namespace infinity;

    ASSERT_NE(nullptr, skiplist_writer_);
    PostingFields posting_fields;

    u8 row_count = 0;
    u32 offset = 0;

    auto posting_field_key = new TypedPostingField<u32>;
    posting_field_key->location_ = row_count++;
    posting_field_key->offset_ = offset;
    posting_field_key->encoder_ = GetSkipListEncoder();
    offset += sizeof(u32);

    auto posting_field_value = new TypedPostingField<u32>;
    posting_field_value->location_ = row_count++;
    posting_field_value->offset_ = offset;
    posting_field_value->encoder_ = GetSkipListEncoder();
    offset += sizeof(u32);

    posting_fields.AddValue(posting_field_key);
    posting_fields.AddValue(posting_field_value);

    skiplist_writer_->Init(&posting_fields);
    const u32 delta = 10;

    String file_path = String(GetTmpDir()) + "/skiplist_writer_test2.txt";
    auto file_writer = CreateFileWriter(file_path);

    Vector<u32> expected_keys;
    Vector<u32> expected_values;

    u32 now_key = 0;
    u32 now_value = 0;

    skiplist_writer_->AddItem(now_key, now_value);
    expected_keys.emplace_back(now_key);
    expected_values.emplace_back(now_value);

    for (int i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        now_key += delta;
        now_value += delta - 1;
        skiplist_writer_->AddItem(now_key, now_value);
        expected_keys.emplace_back(now_key);
        expected_values.emplace_back(now_value);
    }

    skiplist_writer_->Dump(file_writer);

    auto reader = MakeShared<PostingByteSliceReader>();
    reader->Open(skiplist_writer_.get());

    u32 key_buffer[BUFFER_SIZE_];
    u32 val_buffer[BUFFER_SIZE_];
    SizeT decode_len;

    ASSERT_TRUE(reader->Decode(key_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode(val_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode(key_buffer + SKIP_LIST_BUFFER_SIZE, 3, decode_len));
    ASSERT_TRUE(reader->Decode(val_buffer + SKIP_LIST_BUFFER_SIZE, 3, decode_len));

    u32 read_key = 0;
    u32 read_value = 0;
    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        read_key += key_buffer[i];
        read_value = val_buffer[i];
        ASSERT_EQ(read_key, expected_keys[i]);
        ASSERT_EQ(read_value, expected_values[i]);
    }
}

TEST_F(SkipListWriterTest, test3) {
    using namespace infinity;

    ASSERT_NE(nullptr, skiplist_writer_);
    PostingFields posting_fields;

    u8 row_count = 0;
    u32 offset = 0;

    auto posting_field_key = new TypedPostingField<u32>;
    posting_field_key->location_ = row_count++;
    posting_field_key->offset_ = offset;
    posting_field_key->encoder_ = GetSkipListEncoder();

    offset += sizeof(u32);

    auto posting_field_value1 = new TypedPostingField<u32>;
    posting_field_value1->location_ = row_count++;
    posting_field_value1->offset_ = offset;
    posting_field_value1->encoder_ = GetSkipListEncoder();

    offset += sizeof(u32);

    auto posting_field_value2 = new TypedPostingField<u32>;
    posting_field_value2->location_ = row_count++;
    posting_field_value2->offset_ = offset;
    posting_field_value2->encoder_ = GetSkipListEncoder();

    offset += sizeof(u32);

    posting_fields.AddValue(posting_field_key);
    posting_fields.AddValue(posting_field_value1);
    posting_fields.AddValue(posting_field_value2);

    skiplist_writer_->Init(&posting_fields);
    const u32 delta = 10;

    String file_path = String(GetTmpDir()) + "/skiplist_writer_test3.txt";
    auto file_writer = CreateFileWriter(file_path);

    Vector<u32> expected_keys;
    Vector<u32> expected_values1;
    Vector<u32> expected_values2;

    u32 now_key = 0;
    u32 now_value1 = 0;
    u32 now_value2 = 0;
    skiplist_writer_->AddItem(now_key, now_value1, now_value2);
    expected_keys.emplace_back(now_key);
    expected_values1.emplace_back(now_value1);
    expected_values2.emplace_back(now_value2);

    for (int i = 0; i < SKIP_LIST_BUFFER_SIZE + 2; ++i) {
        now_key += delta;
        now_value1 += delta - 1;
        now_value2 += delta - 2;
        skiplist_writer_->AddItem(now_key, now_value1, now_value2);
        expected_keys.emplace_back(now_key);
        expected_values1.emplace_back(now_value1);
        expected_values2.emplace_back(now_value2);
    }

    skiplist_writer_->Dump(file_writer);

    auto reader = MakeShared<PostingByteSliceReader>();
    reader->Open(skiplist_writer_.get());

    u32 key_buffer[BUFFER_SIZE_];
    u32 val1_buffer[BUFFER_SIZE_];
    u32 val2_buffer[BUFFER_SIZE_];
    SizeT decode_len;

    ASSERT_TRUE(reader->Decode(key_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode(val1_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode(val2_buffer, SKIP_LIST_BUFFER_SIZE, decode_len));
    ASSERT_TRUE(reader->Decode(key_buffer + SKIP_LIST_BUFFER_SIZE, 3, decode_len));
    ASSERT_TRUE(reader->Decode(val1_buffer + SKIP_LIST_BUFFER_SIZE, 3, decode_len));
    ASSERT_TRUE(reader->Decode(val2_buffer + SKIP_LIST_BUFFER_SIZE, 3, decode_len));

    u32 read_key = 0;
    u32 read_value1 = 0;
    u32 read_value2 = 0;
    for (size_t i = 0; i < SKIP_LIST_BUFFER_SIZE + 3; ++i) {
        read_key += key_buffer[i];
        read_value1 += val1_buffer[i];
        read_value2 = val2_buffer[i];
        ASSERT_EQ(read_key, expected_keys[i]);
        ASSERT_EQ(read_value1, expected_values1[i]);
        ASSERT_EQ(read_value2, expected_values2[i]);
    }
}
