#include "type/complex/row_id.h"
#include "unit_test/base_test.h"

import stl;
import posting_merger;
import memory_pool;
import memory_indexer;
import segment_term_posting;
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
import third_party;

using namespace infinity;
class ColumnIndexMergerTest : public BaseTest {
public:
    ColumnIndexMergerTest() {
        memory_pool_ = new MemoryPool(BUFFER_SIZE_);
        buffer_pool_ = new RecyclePool(BUFFER_SIZE_);
        byte_slice_pool_ = new MemoryPool(BUFFER_SIZE_);
    }

    ~ColumnIndexMergerTest() {
        delete memory_pool_;
        delete buffer_pool_;
        delete byte_slice_pool_;
    }

    void SetUp() override {
        system("rm -rf /tmp/infinity/column_index_merger");
        system("mkdir -p /tmp/infinity/column_index_merger");
    }

    void TearDown() override {}

public:
    struct ExpectedPosting {
        String term;
        Vector<RowID> doc_ids;
        Vector<u32> tfs;
    };

protected:
    void CreateIndex(const char *paragraphs[], const SizeT num_paragraph, const String& index_dir, const Vector<String>& chunk_names,
                     const Vector<RowID>& base_row_ids, const Vector<u32>& row_offsets, const Vector<u32>& row_counts);
protected:
    MemoryPool *memory_pool_;
    RecyclePool *buffer_pool_;

    MemoryPool *byte_slice_pool_;
    ThreadPool thread_pool_{4};
    optionflag_t flag_{OPTION_FLAG_ALL};
    static constexpr SizeT BUFFER_SIZE_ = 1024;
};

void ColumnIndexMergerTest::CreateIndex(const char *paragraphs[],
                                        const SizeT num_paragraph,
                                        const String& index_dir,
                                        const Vector<String>& chunk_names,
                                        const Vector<RowID>& base_row_ids,
                                        const Vector<u32>& row_offsets,
                                        const Vector<u32>& row_counts) {

    EXPECT_EQ(chunk_names.size(), base_row_ids.size());
    EXPECT_EQ(chunk_names.size(), row_offsets.size());
    EXPECT_EQ(chunk_names.size(), row_counts.size());

    SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    column->Initialize();
    for (SizeT i = 0; i < num_paragraph; ++i) {
        Value v = Value::MakeVarchar(String(paragraphs[i]));
        column->AppendValue(v);
    }

    auto fake_segment_index_entry_1 = SegmentIndexEntry::CreateFakeEntry();
    for (SizeT i = 0; i < chunk_names.size(); ++i) {
        String column_length_file_path = index_dir + "/" + chunk_names[i] + LENGTH_SUFFIX;
        auto column_length_file_handler =
            MakeShared<FullTextColumnLengthFileHandler>(MakeUnique<LocalFileSystem>(), column_length_file_path, fake_segment_index_entry_1.get());
        MemoryIndexer
            indexer(index_dir, chunk_names[i], base_row_ids[i], flag_, "standard", *byte_slice_pool_, *buffer_pool_, thread_pool_);
        indexer.Insert(column, row_offsets[i], row_counts[i], column_length_file_handler);
        indexer.Dump();
    }
}



TEST_F(ColumnIndexMergerTest, BasicParagraphTest) {
    using namespace infinity;
    const char *paragraphs[] = {
        R"#(B A)#",
        R"#(A B A)#",
        R"#(A A A)#"
    };
    const SizeT num_paragraph = sizeof(paragraphs) / sizeof(char *);
    const String index_dir = "/tmp/infinity/column_index_merger";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 2U}};
    Vector<u32> row_offsets = {0, 2};
    Vector<u32> row_counts = {2, 1};
    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);

    String dst_base_name = "merged_index";

    auto column_index_merger = MakeShared<ColumnIndexMerger>(index_dir, flag_, memory_pool_, buffer_pool_);

    column_index_merger->Merge(base_names, base_row_ids, dst_base_name);

    auto fake_segment_index_entry_1 = SegmentIndexEntry::CreateFakeEntry();
    fake_segment_index_entry_1->GetFulltextBaseNames().emplace_back(dst_base_name);
    fake_segment_index_entry_1->GetFulltextBaseRowIDs().emplace_back(RowID(0U, 0U).ToUint64());
    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = {{1, fake_segment_index_entry_1}};
    ColumnIndexReader reader;
    auto dir = index_dir;
    reader.Open(flag_, std::move(dir), std::move(index_by_segment));

    Vector<ExpectedPosting> expected_postings = {{"a", {0, 1, 2}, {1, 2, 3}}, {"b", {0, 1}, {1, 1}}};

    for (SizeT i = 0; i < expected_postings.size(); ++i) {
        const ExpectedPosting &expected = expected_postings[i];
        const String &term = expected.term;

        UniquePtr<PostingIterator> post_iter(reader.Lookup(term, byte_slice_pool_));
        ASSERT_TRUE(post_iter != nullptr);

        RowID doc_id = INVALID_ROWID;
        for (SizeT j = 0; j < expected.doc_ids.size(); ++j) {
            doc_id = post_iter->SeekDoc(expected.doc_ids[j]);
            ASSERT_EQ(doc_id, expected.doc_ids[j]);
            u32 tf = post_iter->GetCurrentTF();
            ASSERT_EQ(tf, expected.tfs[j]);
        }
        if (doc_id != INVALID_ROWID) {
            doc_id = post_iter->SeekDoc(doc_id + 1);
            ASSERT_EQ(doc_id, INVALID_ROWID);
        }
    }
}

TEST_F(ColumnIndexMergerTest, BasicParagraphTest2) {
    using namespace infinity;
    // https://en.wikipedia.org/wiki/Finite-state_transducer
    const char *paragraphs[] = {
        R"#(A finite-state transducer (FST) is a finite-state machine with two memory tapes, following the terminology for Turing machines: an input tape and an output tape. This contrasts with an ordinary finite-state automaton, which has a single tape. An FST is a type of finite-state automaton (FSA) that maps between two sets of symbols.[1] An FST is more general than an FSA. An FSA defines a formal language by defining a set of accepted strings, while an FST defines a relation between sets of strings.)#",
        R"#(An FST will read a set of strings on the input tape and generates a set of relations on the output tape. An FST can be thought of as a translator or relater between strings in a set.)#",
        R"#(In morphological parsing, an example would be inputting a string of letters into the FST, the FST would then output a string of morphemes.)#",
        R"#(An automaton can be said to recognize a string if we view the content of its tape as input. In other words, the automaton computes a function that maps strings into the set {0,1}. Alternatively, we can say that an automaton generates strings, which means viewing its tape as an output tape. On this view, the automaton generates a formal language, which is a set of strings. The two views of automata are equivalent: the function that the automaton computes is precisely the indicator function of the set of strings it generates. The class of languages generated by finite automata is known as the class of regular languages.)#",
        R"#(The two tapes of a transducer are typically viewed as an input tape and an output tape. On this view, a transducer is said to transduce (i.e., translate) the contents of its input tape to its output tape, by accepting a string on its input tape and generating another string on its output tape. It may do so nondeterministically and it may produce more than one output for each input string. A transducer may also produce no output for a given input string, in which case it is said to reject the input. In general, a transducer computes a relation between two formal languages.)#",
    };
    const SizeT num_paragraph = sizeof(paragraphs) / sizeof(char *);
    const String index_dir = "/tmp/infinity/column_index_merger";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 1U}};
    Vector<u32> row_offsets = {0, 1};
    Vector<u32> row_counts = {1, 4};
    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);

    String dst_base_name = "merged_index";

    auto column_index_merger = MakeShared<ColumnIndexMerger>(index_dir, flag_, memory_pool_, buffer_pool_);

    column_index_merger->Merge(base_names, base_row_ids, dst_base_name);

    auto fake_segment_index_entry_1 = SegmentIndexEntry::CreateFakeEntry();
    fake_segment_index_entry_1->GetFulltextBaseNames().emplace_back(dst_base_name);
    fake_segment_index_entry_1->GetFulltextBaseRowIDs().emplace_back(RowID(0U, 0U).ToUint64());
    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = {{1, fake_segment_index_entry_1}};
    ColumnIndexReader reader;
    auto dir = index_dir;
    reader.Open(flag_, std::move(dir), std::move(index_by_segment));

    Vector<ExpectedPosting> expected_postings = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};

    for (SizeT i = 0; i < expected_postings.size(); ++i) {
        const ExpectedPosting &expected = expected_postings[i];
        const String &term = expected.term;

        UniquePtr<PostingIterator> post_iter(reader.Lookup(term, byte_slice_pool_));
        ASSERT_TRUE(post_iter != nullptr);

        RowID doc_id = INVALID_ROWID;
        for (SizeT j = 0; j < expected.doc_ids.size(); ++j) {
            doc_id = post_iter->SeekDoc(expected.doc_ids[j]);
            ASSERT_EQ(doc_id, expected.doc_ids[j]);
            u32 tf = post_iter->GetCurrentTF();
            ASSERT_EQ(tf, expected.tfs[j]);
        }
        if (doc_id != INVALID_ROWID) {
            doc_id = post_iter->SeekDoc(doc_id + 1);
            ASSERT_EQ(doc_id, INVALID_ROWID);
        }
    }
}