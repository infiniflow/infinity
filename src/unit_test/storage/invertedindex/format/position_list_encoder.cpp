
#include "unit_test/base_test.h"

import stl;
import memory_pool;
import index_defines;
import posting_byte_slice;
import position_list_format_option;
import position_list_encoder;
import skiplist_writer;
import posting_byte_slice_reader;
import file_writer;
import file_reader;
import local_file_system;

using namespace infinity;

class PositionListEncoderTest : public BaseTest {
public:
    PositionListEncoderTest() : buffer_pool_(10240), byte_slice_pool_(10240) {}

    void SetUp() override { file_ = "/tmp/position"; }
    void TearDown() override {}

protected:
    String file_;
    MemoryPool buffer_pool_;
    MemoryPool byte_slice_pool_;
    LocalFileSystem fs_;
};

TEST_F(PositionListEncoderTest, test1) {
    optionflag_t option_flag = of_position_list | of_term_frequency;
    PositionListFormatOption format_option(option_flag);
    PositionListEncoder position_encoder(format_option, &byte_slice_pool_, &buffer_pool_);

    for (u32 i = 0; i <= MAX_DOC_PER_RECORD + 1; i++) {
        position_encoder.AddPosition(i);
    }
    position_encoder.EndDocument();
    SharedPtr<FileWriter> file_writer = MakeShared<FileWriter>(fs_, file_, 128);
    position_encoder.Dump(file_writer);
    file_writer->Sync();

    fs_.DeleteFile(file_);
}

TEST_F(PositionListEncoderTest, test2) {
    optionflag_t option_flag = of_position_list | of_term_frequency;
    PositionListFormatOption format_option(option_flag);
    PositionListEncoder position_encoder(format_option, &byte_slice_pool_, &buffer_pool_);

    for (u32 i = 1; i <= MAX_DOC_PER_RECORD + 1; i++) {
        position_encoder.AddPosition(i);
    }
    position_encoder.EndDocument();
    PostingByteSliceReader reader;
    reader.Open(position_encoder.GetBufferedByteSlice());

    pos_t pos_buffer[MAX_POS_PER_RECORD];
    SizeT decode_count = 0;
    ASSERT_TRUE(reader.Decode(pos_buffer, MAX_POS_PER_RECORD, decode_count));
    ASSERT_EQ(MAX_POS_PER_RECORD, decode_count);
    for (u32 i = 0; i < MAX_POS_PER_RECORD; i++) {
        ASSERT_EQ((pos_t)1, pos_buffer[i]);
    }
    ASSERT_TRUE(reader.Decode(pos_buffer, MAX_POS_PER_RECORD, decode_count));
    ASSERT_EQ((SizeT)1, decode_count);
    ASSERT_TRUE(!reader.Decode(pos_buffer, MAX_POS_PER_RECORD, decode_count));
}
