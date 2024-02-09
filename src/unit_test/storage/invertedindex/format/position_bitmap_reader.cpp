#include "unit_test/base_test.h"

import stl;
import memory_pool;
import buffered_byte_slice;
import position_bitmap_reader;
import position_bitmap_writer;
import file_writer;
import file_reader;
import local_file_system;
import byte_slice;
import byte_slice_reader;
import byte_slice_writer;
import index_defines;

using namespace infinity;

class PositionBitmapReaderTest : public BaseTest {
public:
    void SetUp() override { dir_ = "/tmp/"; }
    void TearDown() override {}

    void DoTest(u32 count, bool random) {
        Vector<std::pair<u32, u32>> answer;

        MemoryPool pool;
        PositionBitmapWriter writer(&pool);
        u32 pos_count = 0;
        for (u32 i = 0; i < count; ++i) {
            std::pair<u32, u32> one_answer;
            one_answer.first = pos_count;
            writer.Set(pos_count);
            pos_count++;
            if (random) {
                int r = rand() % 10;
                pos_count += r;
                one_answer.second = r + 1;
            } else {
                pos_count += i;
                one_answer.second = i + 1;
            }
            writer.EndDocument(i + 1, pos_count);
            answer.push_back(one_answer);
        }
        writer.Resize(pos_count);

        String file_name = dir_ + "bitmap";

        SharedPtr<FileWriter> file_writer = MakeShared<FileWriter>(fs_, file_name, 128);
        writer.Dump(file_writer, pos_count);
        file_writer->Sync();

        u32 file_length = writer.GetDumpLength(pos_count);

        FileReader file_reader(fs_, file_name, 128);
        ByteSlice *slice = ByteSlice::CreateSlice(file_length, &pool);
        file_reader.Read((char *)slice->data_, file_length);
        ByteSliceList byte_slice_list(slice, &pool);
        PositionCountInfo info;
        PositionBitmapReader reader;
        reader.Init(&byte_slice_list, 0);
        for (u32 i = 0; i < count; i++) {
            info = reader.GetPosCountInfo(i + 1);
            ASSERT_EQ(info.pre_doc_agg_pos_count_, answer[i].first);
            ASSERT_EQ(info.current_doc_pos_count_, answer[i].second);
        }
        fs_.DeleteFile(file_name);
    }

    void DoTest2() {
        MemoryPool pool;
        PositionBitmapWriter writer(&pool);

        /*
         * first block:
         * 100111111....11111111111
         *    |<------255-------->|
         */
        writer.Set(0);
        writer.EndDocument(1, 3);
        for (u32 i = 0; i < 255; ++i) {
            writer.Set(i + 3);
            writer.EndDocument(i + 2, i + 4);
        }
        /*
         * second block:
         * 111111111111111111111111
         * |<--------100--------->|
         */
        for (u32 i = 0; i < 100; ++i) {
            writer.Set(i + 258);
            writer.EndDocument(i + 257, i + 259);
        }
        u32 pos_count = 358;
        writer.Resize(pos_count);

        String file_name = dir_ + "bitmap";
        SharedPtr<FileWriter> file_writer = MakeShared<FileWriter>(fs_, file_name, 128);
        writer.Dump(file_writer, pos_count);
        file_writer->Sync();

        u32 file_length = writer.GetDumpLength(pos_count);

        FileReader file_reader(fs_, file_name, 128);
        ByteSlice *slice = ByteSlice::CreateSlice(file_length, &pool);
        file_reader.Read((char *)slice->data_, file_length);
        ByteSliceList byte_slice_list(slice, &pool);

        PositionCountInfo info;
        PositionBitmapReader reader;
        reader.Init(&byte_slice_list, 0);

        info = reader.GetPosCountInfo(300);
        ASSERT_EQ((u32)301, info.pre_doc_agg_pos_count_);
        ASSERT_EQ((u32)1, info.current_doc_pos_count_);

        info = reader.GetPosCountInfo(356);
        ASSERT_EQ((u32)357, info.pre_doc_agg_pos_count_);
        ASSERT_EQ((u32)1, info.current_doc_pos_count_);

        fs_.DeleteFile(file_name);
    }

private:
    String dir_;
    LocalFileSystem fs_;
};

TEST_F(PositionBitmapReaderTest, test1) {
    DoTest(10, false);
    DoTest(10, true);

    DoTest(MAX_DOC_PER_BITMAP_BLOCK - 1, false);
    DoTest(MAX_DOC_PER_BITMAP_BLOCK - 1, true);
    DoTest(MAX_DOC_PER_BITMAP_BLOCK, false);
    DoTest(MAX_DOC_PER_BITMAP_BLOCK, true);

    DoTest(MAX_DOC_PER_BITMAP_BLOCK + 1, false);
    DoTest(MAX_DOC_PER_BITMAP_BLOCK + 1, true);

    DoTest(MAX_DOC_PER_BITMAP_BLOCK * 3 + 10, false);
    DoTest(MAX_DOC_PER_BITMAP_BLOCK * 3 + 10, true);
}

TEST_F(PositionBitmapReaderTest, test2) { DoTest2(); }