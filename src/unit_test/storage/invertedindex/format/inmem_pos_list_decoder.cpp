#include "unit_test/base_test.h"

import stl;
import memory_pool;
import index_defines;
import buffered_byte_slice;
import inmem_pos_list_decoder;
import doc_list_format_option;
import pair_value_skiplist_reader;
import pos_list_encoder;
import in_doc_pos_state;
import pos_list_format_option;

using namespace infinity;

class InMemPositionListDecoderTest : public BaseTest {
public:
    InMemPositionListDecoderTest() : buffer_pool_(10240), byte_slice_pool_(10240) {}

    void SetUp() override {}
    void TearDown() override {}

protected:
    void TestDecodeWithOptionFlag(const optionflag_t flag, tf_t tf, pos_t *pos_list, bool need_flush) {
        PositionListFormatOption option(flag);

        PositionListEncoder pos_list_encoder(option, &byte_slice_pool_, &buffer_pool_);
        for (SizeT i = 0; i < (SizeT)tf; ++i) {
            pos_list_encoder.AddPosition(pos_list[i]);
        }
        pos_list_encoder.EndDocument();
        if (need_flush) {
            pos_list_encoder.Flush();
        }

        UniquePtr<InMemPositionListDecoder> pos_list_decoder(pos_list_encoder.GetInMemPositionListDecoder(&byte_slice_pool_));
        ASSERT_TRUE(pos_list_decoder);

        // compress mode is useless in decoder
        InDocPositionState state(option);
        ASSERT_TRUE(pos_list_decoder->SkipTo(0, &state));

        u32 tempTF = 0;
        ASSERT_TRUE(pos_list_decoder->LocateRecord(&state, tempTF));
        ASSERT_EQ((i32)0, state.GetRecordOffset());
        ASSERT_EQ((i32)0, state.GetOffsetInRecord());

        pos_t pos_buffer[MAX_POS_PER_RECORD];

        SizeT pos_idx = 0;
        pos_t last_pos = 0;
        for (SizeT i = 0; i < tf / MAX_POS_PER_RECORD; ++i) {
            u32 decodeCount = pos_list_decoder->DecodeRecord(pos_buffer, MAX_POS_PER_RECORD);
            ASSERT_EQ((u32)MAX_POS_PER_RECORD, decodeCount);
            for (SizeT j = 0; j < MAX_POS_PER_RECORD; ++j) {
                ASSERT_EQ(pos_list[pos_idx], last_pos + pos_buffer[j]);
                last_pos = last_pos + pos_buffer[j];
                pos_idx++;
            }
        }
        if (tf % MAX_POS_PER_RECORD != 0) {
            u32 decodeCount = pos_list_decoder->DecodeRecord(pos_buffer, MAX_POS_PER_RECORD);
            ASSERT_EQ((u32)(tf % MAX_POS_PER_RECORD), decodeCount);
            for (SizeT j = 0; j < decodeCount; ++j) {
                ASSERT_EQ(pos_list[pos_idx], last_pos + pos_buffer[j]);
                last_pos = last_pos + pos_buffer[j];
                pos_idx++;
            }
        }
    }

    void InnerTestDecode(const optionflag_t flag, tf_t tf) {
        pos_t *pos_list = new pos_t[tf];

        for (tf_t i = 0; i < tf; ++i) {
            pos_list[i] = i * 2 + 1;
        }

        TestDecodeWithOptionFlag(flag, tf, pos_list, false);
        TestDecodeWithOptionFlag(flag, tf, pos_list, true);

        delete[] pos_list;
    }

    void InnerTestSkipAndLocateAndDecode(optionflag_t flag, ttf_t ttf, bool need_flush) {
        PositionListFormatOption option(flag);

        PositionListEncoder pos_list_encoder(option, &byte_slice_pool_, &buffer_pool_);
        pos_t sum = 0;
        for (SizeT i = 0; i < (SizeT)ttf; ++i) {
            sum += i;
            pos_list_encoder.AddPosition(sum);
        }
        pos_list_encoder.EndDocument();
        if (need_flush) {
            pos_list_encoder.Flush();
        }

        UniquePtr<InMemPositionListDecoder> pos_list_decoder(pos_list_encoder.GetInMemPositionListDecoder(&byte_slice_pool_));
        ASSERT_TRUE(pos_list_decoder);

        // compress mode is useless in decoder
        InDocPositionState state(option);

        i32 recordOffset = 0;
        u32 tempTF = 0;
        pos_t pos_buffer[MAX_POS_PER_RECORD];
        for (SizeT i = 0; i < (SizeT)ttf; ++i) {
            ASSERT_TRUE(pos_list_decoder->SkipTo(i, &state));
            if (i % MAX_POS_PER_RECORD == 0) {
                ASSERT_TRUE(pos_list_decoder->LocateRecord(&state, tempTF));
                recordOffset = state.GetRecordOffset();
                // check decode buffer
                u32 decodeCount = pos_list_decoder->DecodeRecord(pos_buffer, MAX_POS_PER_RECORD);
                u32 expectDecodeCount = std::min(u32(ttf - i), MAX_POS_PER_RECORD);
                ASSERT_EQ(expectDecodeCount, decodeCount);
                for (SizeT j = 0; j < (SizeT)decodeCount; ++j) {
                    ASSERT_EQ(i + j, pos_buffer[j]);
                }
            } else {
                ASSERT_FALSE(pos_list_decoder->LocateRecord(&state, tempTF));
                ASSERT_EQ(recordOffset, state.GetRecordOffset());
            }
            ASSERT_EQ((i32)(i % MAX_POS_PER_RECORD), state.GetOffsetInRecord());
        }
    }

    RecyclePool buffer_pool_;
    MemoryPool byte_slice_pool_;
};

TEST_F(InMemPositionListDecoderTest, test1) {
    using namespace infinity;

    constexpr optionflag_t NO_PAYLOAD = of_position_list | of_term_frequency;
    PositionListFormatOption option(NO_PAYLOAD);

    PositionListEncoder pos_list_encoder(option, &byte_slice_pool_, &buffer_pool_);
    pos_list_encoder.AddPosition(3);
    pos_list_encoder.EndDocument();

    UniquePtr<InMemPositionListDecoder> posListDecoder(pos_list_encoder.GetInMemPositionListDecoder(&byte_slice_pool_));
    ASSERT_TRUE(posListDecoder);

    InDocPositionState state(option);
    ASSERT_TRUE(posListDecoder->SkipTo(0, &state));

    u32 tf = 0;
    ASSERT_TRUE(posListDecoder->LocateRecord(&state, tf));
    ASSERT_EQ((i32)0, state.GetRecordOffset());
    ASSERT_EQ((i32)0, state.GetOffsetInRecord());

    pos_t posBuffer[MAX_POS_PER_RECORD];
    ASSERT_EQ((u32)1, posListDecoder->DecodeRecord(posBuffer, MAX_POS_PER_RECORD));
    ASSERT_EQ((pos_t)3, posBuffer[0]);
}

TEST_F(InMemPositionListDecoderTest, test2) {
    using namespace infinity;

    const optionflag_t flag = of_position_list;
    tf_t tf = 5;
    pos_t posList[] = {1, 5, 6, 19, 20};

    TestDecodeWithOptionFlag(flag, tf, posList, false);
    TestDecodeWithOptionFlag(flag, tf, posList, true);
}

TEST_F(InMemPositionListDecoderTest, test3) {
    using namespace infinity;

    const optionflag_t flag = of_position_list;
    InnerTestDecode(flag, 1);
    InnerTestDecode(flag, MAX_UNCOMPRESSED_POS_LIST_SIZE + 1);
    InnerTestDecode(flag, MAX_POS_PER_RECORD);
    InnerTestDecode(flag, MAX_POS_PER_RECORD + 10);
    InnerTestDecode(flag, MAX_POS_PER_RECORD * 5 + 1);
    InnerTestDecode(flag, MAX_POS_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1);
    InnerTestDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1);
    InnerTestDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1);
}

TEST_F(InMemPositionListDecoderTest, test4) {
    using namespace infinity;

    PositionListFormatOption option(of_position_list);
    InDocPositionState state(option);

    BufferedByteSlice *pos_list_buffer =
        new (byte_slice_pool_.Allocate(sizeof(BufferedByteSlice))) BufferedByteSlice(&byte_slice_pool_, &buffer_pool_);
    InMemPositionListDecoder decoder(option, &byte_slice_pool_);
    decoder.Init(100, NULL, pos_list_buffer);

    // current ttf over totalTF
    ASSERT_FALSE(decoder.SkipTo(101, &state));
    ASSERT_TRUE(decoder.SkipTo(50, &state));

    ASSERT_EQ((i32)0, state.GetRecordOffset());
    ASSERT_EQ((i32)50, state.GetOffsetInRecord());
}

TEST_F(InMemPositionListDecoderTest, test5) {
    using namespace infinity;

    const optionflag_t flag = of_position_list;

    InnerTestSkipAndLocateAndDecode(flag, 1, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_UNCOMPRESSED_POS_LIST_SIZE + 1, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD + 10, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * 5 + 1, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1, true);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1, true);

    InnerTestSkipAndLocateAndDecode(flag, 1, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_UNCOMPRESSED_POS_LIST_SIZE + 1, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD + 10, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * 5 + 1, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1, false);
    InnerTestSkipAndLocateAndDecode(flag, MAX_POS_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1, false);
}
