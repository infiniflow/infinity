
#include "unit_test/base_test.h"

import stl;

import file_reader;
import file_writer;
import index_defines;
import local_file_system;
import position_list_encoder;
import position_list_decoder;
import posting_list_format;
import posting_byte_slice_reader;
import in_doc_pos_state;
import posting_byte_slice;
import byte_slice_reader;

using namespace infinity;

class PositionListDecoderTest : public BaseTest {
public:
    PositionListDecoderTest() {}

    ~PositionListDecoderTest() {}

    void SetUp() override {}
    void TearDown() override {}

protected:
    static constexpr SizeT BUFFER_SIZE_ = 10240;
};

TEST_F(PositionListDecoderTest, test1) {
    optionflag_t option_flag = of_position_list | of_term_frequency;
    PostingFormatOption format_option(option_flag);
    PositionListEncoder position_encoder(format_option);

    for (u32 i = 0; i < MAX_DOC_PER_RECORD + 2; i++) {
        position_encoder.AddPosition(i);
    }
    position_encoder.EndDocument();

    pos_t pos_buffer[BUFFER_SIZE_];

    // It seems that PositionListDecoder cannot be used directly for PositionListEncoder, so its subclass InMemPositionListDecoder is used for testing here.
    PositionListDecoder *position_decoder = position_encoder.GetInMemPositionListDecoder();

    auto pos_count1 = position_decoder->DecodeRecord(pos_buffer, MAX_DOC_PER_RECORD);
    auto pos_count2 = position_decoder->DecodeRecord(pos_buffer + MAX_POS_PER_RECORD, 2);
    ASSERT_EQ(pos_count1, MAX_POS_PER_RECORD);
    ASSERT_EQ(pos_count2, static_cast<u32>(2));
    ASSERT_EQ(pos_count1 + pos_count2, MAX_POS_PER_RECORD + 2);

    for (SizeT i = 0; i < MAX_DOC_PER_RECORD + 2; ++i) {
        pos_t expected_delta = (i > 0);
        ASSERT_EQ(pos_buffer[i], expected_delta);
    }
    delete position_decoder;
}
