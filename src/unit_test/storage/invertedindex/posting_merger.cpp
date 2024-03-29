#include "unit_test/base_test.h"

import posting_merger;
import stl;
import memory_pool;
import segment_term_posting;
import memory_indexer;
import column_vector;
import data_type;
import value;
import column_inverter;
import index_defines;
import column_index_reader;
import posting_iterator;
import file_system;
import local_file_system;
import file_writer;
import term_meta;
import index_defines;
import posting_list_format;
import segment_index_entry;
import column_length_io;
import posting_decoder;
import column_index_merger;
import internal_types;
import logical_type;

using namespace infinity;

class PostingMergerTest : public BaseTest {
public:
    PostingMergerTest() {
        memory_pool_ = new MemoryPool(BUFFER_SIZE_);
        buffer_pool_ = new RecyclePool(BUFFER_SIZE_);
        byte_slice_pool_ = new MemoryPool(BUFFER_SIZE_);
    }
    ~PostingMergerTest() {
        delete memory_pool_;
        delete buffer_pool_;
        delete byte_slice_pool_;
    }
    void SetUp() override {
        system("rm -rf /tmp/infinity/posting_merger");
        system("mkdir -p /tmp/infinity/posting_merger");
    }
    void TearDown() override {}

public:
    struct ExpectedPosting {
        String term;
        Vector<RowID> doc_ids;
        Vector<u32> tfs;
    };

protected:
    void CreateIndex();

protected:
    MemoryPool *memory_pool_;
    RecyclePool *buffer_pool_;

    MemoryPool *byte_slice_pool_;
    ThreadPool thread_pool_{4};
    optionflag_t flag_{OPTION_FLAG_ALL};
    static constexpr SizeT BUFFER_SIZE_ = 1024;
};

void PostingMergerTest::CreateIndex() {

    const char *paragraphs[] = {
        R"#(A)#",
        R"#(A A)#",
    };
    const SizeT num_paragraph = sizeof(paragraphs) / sizeof(char *);

    SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    column->Initialize();
    for (SizeT i = 0; i < num_paragraph; ++i) {
        Value v = Value::MakeVarchar(String(paragraphs[i]));
        column->AppendValue(v);
    }

    auto fake_segment_index_entry_1 = SegmentIndexEntry::CreateFakeEntry();
    String column_length_file_path_1 = String("/tmp/infinity/posting_merger/chunk1") + LENGTH_SUFFIX;
    auto column_length_file_handler_1 =
        MakeShared<FullTextColumnLengthFileHandler>(MakeUnique<LocalFileSystem>(), column_length_file_path_1, fake_segment_index_entry_1.get());
    MemoryIndexer
        indexer1("/tmp/infinity/posting_merger", "chunk1", RowID(0U, 0U), flag_, "standard", *byte_slice_pool_, *buffer_pool_, thread_pool_);
    indexer1.Insert(column, 0, 1, column_length_file_handler_1);
    indexer1.Dump();
    fake_segment_index_entry_1->GetFulltextBaseNames().emplace_back("chunk1");
    fake_segment_index_entry_1->GetFulltextBaseRowIDs().emplace_back(RowID(0U, 0U).ToUint64());

    String column_length_file_path_2 = String("/tmp/infinity/posting_merger/chunk2") + LENGTH_SUFFIX;
    auto column_length_file_handler_2 =
        MakeShared<FullTextColumnLengthFileHandler>(MakeUnique<LocalFileSystem>(), column_length_file_path_2, fake_segment_index_entry_1.get());
    auto indexer2 = MakeUnique<MemoryIndexer>("/tmp/infinity/posting_merger",
                                              "chunk2",
                                              RowID(0U, 1U),
                                              flag_,
                                              "standard",
                                              *byte_slice_pool_,
                                              *buffer_pool_,
                                              thread_pool_);
    indexer2->Insert(column, 1, 1, std::move(column_length_file_handler_2));
    indexer2->Dump();
}

TEST_F(PostingMergerTest, Basic) {
    using namespace infinity;
    CreateIndex();

    const String index_dir = "/tmp/infinity/posting_merger";

    String dst_base_name = "merged_index";
    Path path = Path(index_dir) / dst_base_name;
    String index_prefix = path.string();
    String dict_file = index_prefix;
    dict_file.append(DICT_SUFFIX);


    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> row_ids = {RowID{0U, 0U}, RowID{0U, 1U}};
    Vector<SegmentTermPosting *> segment_term_postings;

    Vector<Vector<u32>> expected_pos{{0}, {0, 1}};
    for (size_t i = 0; i < base_names.size(); ++i) {
        auto base_name = base_names[i];
        auto row_id = row_ids[i];
        {
            String expected_term("a");
            auto segment_term_posting1 = MakeUnique<SegmentTermPosting>(index_dir, base_name, row_id, flag_);
            auto column_index_iterator = segment_term_posting1->column_index_iterator_;
            PostingDecoder *decoder;
            String term_str;
            while (column_index_iterator->Next(term_str, decoder)) {
                EXPECT_EQ(term_str, expected_term);
                u32 pos_list_buf[1000];
                auto pos_num = decoder->DecodePosList(pos_list_buf, 1000);
                u32 pos = 0;
                for (u32 j = 0; j < pos_num; ++j) {
                    pos += pos_list_buf[j];
                    EXPECT_EQ(expected_pos[i][j], pos);
                }
            }
        }
        auto segment_term_posting = new SegmentTermPosting(index_dir, base_name, row_id, flag_);
        if (!segment_term_posting->HasNext()) {
            delete segment_term_posting;
        } else {
            segment_term_postings.push_back(segment_term_posting);
        }
    }

    auto posting_merger = MakeShared<PostingMerger>(memory_pool_, buffer_pool_);

    auto merge_base_rowid = row_ids[0];
    for (auto& row_id : row_ids) {
        merge_base_rowid = std::min(merge_base_rowid, row_id);
    }

    posting_merger->Merge(segment_term_postings, merge_base_rowid);
    EXPECT_EQ(posting_merger->GetDF(), static_cast<u32>(2));
    EXPECT_EQ(posting_merger->GetTotalTF(), static_cast<u32>(3));
    for (auto segment_term_posting : segment_term_postings) {
        delete segment_term_posting;
    }
}