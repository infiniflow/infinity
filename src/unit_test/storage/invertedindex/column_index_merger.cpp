#include "type/complex/row_id.h"
#include "unit_test/base_test.h"

import stl;
import posting_merger;

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
import random;
import global_resource_usage;
import infinity_context;

using namespace infinity;
class ColumnIndexMergerTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
        BaseTest::RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }

public:
    ColumnIndexMergerTest() {}

    ~ColumnIndexMergerTest() {}

public:
    struct ExpectedPosting {
        String term;
        Vector<RowID> doc_ids;
        Vector<u32> tfs;
    };

protected:
    void CreateIndex(const char *paragraphs[],
                     const SizeT num_paragraph,
                     const String& index_dir,
                     const Vector<String>& chunk_names,
                     const Vector<RowID>& base_row_ids,
                     const Vector<u32>& row_offsets,
                     const Vector<u32>& row_counts);

    void CreateIndex(const Vector<String>& paragraphs,
                     const String& index_dir,
                     const Vector<String>& chunk_names,
                     const Vector<RowID>& base_row_ids,
                     const Vector<u32>& row_offsets,
                     const Vector<u32>& row_counts);

    void MergeAndCheckIndex(const String& index_dir,
                            const Vector<String>& base_names,
                            const Vector<RowID>& base_row_ids,
                            const String &dst_base_name,
                            const Vector<ExpectedPosting> &expected_postings);

    void GenerateParagraphs(u32 term_num, u32 row_num, u32 word_num, Vector<String>& paragraphs, Vector<ExpectedPosting>& expected_postings);
    void GenerateTerms(Vector<String>& terms, u32 term_num);
    void GenerateExpectedPosting(Map<String, Vector<int>>& term_postings,
                                 Vector<ExpectedPosting>& expected_postings);
    String GetParagraph(const Vector<String>& terms);
    String GetTerm(u32 n);

protected:
    optionflag_t flag_{OPTION_FLAG_ALL};
    static constexpr SizeT BUFFER_SIZE_ = 1024;
};

void ColumnIndexMergerTest::CreateIndex(const Vector<String>& paragraphs,
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
    for (SizeT i = 0; i < paragraphs.size(); ++i) {
        Value v = Value::MakeVarchar(paragraphs[i]);
        column->AppendValue(v);
    }
    for (SizeT i = 0; i < chunk_names.size(); ++i) {
        MemoryIndexer indexer(index_dir, chunk_names[i], base_row_ids[i], flag_, "standard");
        indexer.Insert(column, row_offsets[i], row_counts[i]);
        indexer.Dump();
    }
}

void ColumnIndexMergerTest::CreateIndex(const char *paragraphs[],
                                        const SizeT num_paragraph,
                                        const String& index_dir,
                                        const Vector<String>& chunk_names,
                                        const Vector<RowID>& base_row_ids,
                                        const Vector<u32>& row_offsets,
                                        const Vector<u32>& row_counts) {
    Vector<String> paragraphs_vec(paragraphs, paragraphs + num_paragraph);
    CreateIndex(paragraphs_vec, index_dir, chunk_names, base_row_ids, row_offsets, row_counts);
}

void ColumnIndexMergerTest::MergeAndCheckIndex(const String& index_dir,
                                               const Vector<String>& base_names,
                                               const Vector<RowID>& base_row_ids,
                                               const String &dst_base_name,
                                               const Vector<ExpectedPosting> &expected_postings) {
    // auto column_index_merger = MakeShared<ColumnIndexMerger>(index_dir, flag_);
    // column_index_merger->Merge(base_names, base_row_ids, dst_base_name);

    // auto fake_segment_index_entry_1 = SegmentIndexEntry::CreateFakeEntry(index_dir);
    // fake_segment_index_entry_1->AddFtChunkIndexEntry(dst_base_name, RowID(0U, 0U), 0U);
    // Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = {{0, fake_segment_index_entry_1}};
    // ColumnIndexReader reader;
    // auto dir = index_dir;
    // reader.Open(flag_, std::move(dir), std::move(index_by_segment));

    // for (SizeT i = 0; i < expected_postings.size(); ++i) {
    //     const ExpectedPosting &expected = expected_postings[i];
    //     const String &term = expected.term;

    //     UniquePtr<PostingIterator> post_iter(reader.Lookup(term));
    //     ASSERT_TRUE(post_iter != nullptr);

    //     RowID doc_id = INVALID_ROWID;
    //     for (SizeT j = 0; j < expected.doc_ids.size(); ++j) {
    //         doc_id = post_iter->SeekDoc(expected.doc_ids[j]);
    //         ASSERT_EQ(doc_id, expected.doc_ids[j]);
    //         u32 tf = post_iter->GetCurrentTF();
    //         ASSERT_EQ(tf, expected.tfs[j]);
    //     }
    //     if (doc_id != INVALID_ROWID) {
    //         doc_id = post_iter->SeekDoc(doc_id + 1);
    //         ASSERT_EQ(doc_id, INVALID_ROWID);
    //     }
    // }
}

String ColumnIndexMergerTest::GetTerm(u32 n) {
    String result;
    while (n > 0) {
        int remainder = n % 26;
        if (remainder == 0) {
            result = 'z' + result;
            n = n / 26 - 1;
        } else {
            result = char('a' + remainder - 1) + result;
            n = n / 26;
        }
    }
    return result;
}

void ColumnIndexMergerTest::GenerateTerms(Vector<infinity::String> &terms, infinity::u32 term_num) {
    for (u32 i = 1; i <= term_num; ++i) {
        terms.emplace_back(GetTerm(i));
    }
}

String ColumnIndexMergerTest::GetParagraph(const Vector<String>& terms) {
    String paragraph;
    for (const auto& term : terms) {
        paragraph.append(term);
        paragraph.append(" ");
    }
    return paragraph;
}

void ColumnIndexMergerTest::GenerateParagraphs(u32 term_num, u32 row_num, u32 word_num,
                                               Vector<String> &paragraphs, Vector<ExpectedPosting>& expected_postings) {
    Vector<String> terms;
    GenerateTerms(terms, term_num);

    Map<String, Vector<int>> term_postings;
    for (SizeT i = 0; i < terms.size(); ++i) {
        Vector<int> posting(row_num, 0);
        term_postings[terms[i]] = posting;
    }

    for (u32 i = 0; i < row_num; ++i) {
        Vector<String> all_terms;
        for (u32 j = 0; j < word_num; ++j) {
            u32 term_id = random() % term_num;
            auto term = terms[term_id];
            all_terms.emplace_back(terms[term_id]);
            auto& posting = term_postings[term];
            posting[i]++;
        }
        paragraphs.emplace_back(GetParagraph(all_terms));
    }

    GenerateExpectedPosting(term_postings, expected_postings);
}

void ColumnIndexMergerTest::GenerateExpectedPosting(Map<String, Vector<int>> &term_postings,
                                                    Vector<ExpectedPosting> &expected_postings) {
    for (auto& [term, posting] : term_postings) {
        Vector<u32> tfs;
        Vector<RowID> doc_ids;
        for (SizeT i = 0; i < posting.size(); ++i) {
            auto term_count = posting[i];
            if (!term_count) continue;
            tfs.emplace_back(term_count);
            doc_ids.emplace_back(RowID(0, static_cast<u32>(i)));
        }
        if (tfs.empty()) {
            continue;
        }
        expected_postings.emplace_back(ExpectedPosting{term, doc_ids, tfs});
    }
}

TEST_F(ColumnIndexMergerTest, GenerateParagraphsTest) {
    Vector<String> paragraphs;
    Vector<ExpectedPosting> expected_postings;

    u32 term_num = 30;
    u32 row_num = 10;
    u32 word_num = 10;

    GenerateParagraphs(term_num, row_num, word_num, paragraphs, expected_postings);
    for (auto& expected_posting : expected_postings) {
        EXPECT_EQ(expected_posting.doc_ids.size(), expected_posting.tfs.size());
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
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 2U}};
    Vector<u32> row_offsets = {0, 2};
    Vector<u32> row_counts = {2, 1};
    Vector<ExpectedPosting> expected_postings = {{"a", {0, 1, 2}, {1, 2, 3}}, {"b", {0, 1}, {1, 1}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, BasicParagraphTest1) {
    using namespace infinity;
    const char *paragraphs[] = {
        R"#(B A)#",
        R"#(A B A)#",
    };
    const SizeT num_paragraph = sizeof(paragraphs) / sizeof(char *);
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 1U}};
    Vector<u32> row_offsets = {0, 1};
    Vector<u32> row_counts = {1, 1};
    Vector<ExpectedPosting> expected_postings = {{"a", {0, 1}, {1, 2}}, {"b", {0, 1}, {1, 1}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
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
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 1U}};
    Vector<u32> row_offsets = {0, 1};
    Vector<u32> row_counts = {1, 4};
    Vector<ExpectedPosting> expected_postings = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, BasicParagraphTest3) {
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
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2", "chunk3"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 1U}, RowID{0, 3}};
    Vector<u32> row_offsets = {0, 1, 3};
    Vector<u32> row_counts = {1, 2, 2};
    Vector<ExpectedPosting> expected_postings = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, BasicParagraphTest4) {
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
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 2U}};
    Vector<u32> row_offsets = {0, 2};
    Vector<u32> row_counts = {2, 3};
    Vector<ExpectedPosting> expected_postings = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, BasicParagraphTest5) {
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
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 3U}};
    Vector<u32> row_offsets = {0, 3};
    Vector<u32> row_counts = {3, 2};
    Vector<ExpectedPosting> expected_postings = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};

    CreateIndex(paragraphs, num_paragraph, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}


TEST_F(ColumnIndexMergerTest, GeneratePargraphsMergeTest) {
    using namespace infinity;
    Vector<String> paragraphs;
    Vector<ExpectedPosting> expected_postings;

    u32 term_num = 3;
    u32 row_num = 5;
    u32 word_num_pre_row = 5;

    GenerateParagraphs(term_num, row_num, word_num_pre_row, paragraphs, expected_postings);
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 0U}, RowID{0U, 2U}};
    Vector<u32> row_offsets = {0, 2};
    Vector<u32> row_counts = {2, 3};

    CreateIndex(paragraphs, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, GeneratePargraphsMergeTest1) {
    using namespace infinity;
    Vector<String> paragraphs;
    Vector<ExpectedPosting> expected_postings;

    u32 term_num = 100;
    u32 row_num = 40;
    u32 word_num_pre_row = 40;

    GenerateParagraphs(term_num, row_num, word_num_pre_row, paragraphs, expected_postings);
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> base_row_ids = {RowID{0U, 10U}, RowID{0U, 0U}};
    Vector<u32> row_offsets = {10, 0};
    Vector<u32> row_counts = {30, 10};

    CreateIndex(paragraphs, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

// #define LOCAL_MERGER_TEST
#ifdef LOCAL_MERGER_TEST
TEST_F(ColumnIndexMergerTest, GeneratePargraphsMergeTest2) {
    using namespace infinity;
    Vector<String> paragraphs;
    Vector<ExpectedPosting> expected_postings;

    u32 term_num = 100;
    u32 row_num = 200;
    u32 word_num_pre_row = 100;

    GenerateParagraphs(term_num, row_num, word_num_pre_row, paragraphs, expected_postings);
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    u32 base_num = 10;
    u32 row_num_per_base = row_num / base_num;
    Vector<String> base_names;
    Vector<RowID> base_row_ids;
    Vector<u32> row_offsets;
    Vector<u32> row_counts;

    for (u32 i = 0; i < base_num; ++i) {
        base_names.emplace_back(String("chunk") + std::to_string(i + 1));
        base_row_ids.emplace_back(RowID{0U, i * row_num_per_base});
        row_offsets.emplace_back(i * row_num_per_base);
        if (i == base_num - 1) {
            row_counts.emplace_back(row_num - i * row_num_per_base);
        } else {
            row_counts.emplace_back(row_num_per_base);
        }
    }

    CreateIndex(paragraphs, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}

TEST_F(ColumnIndexMergerTest, GeneratePargraphsMergeTest3) {
    using namespace infinity;
    Vector<String> paragraphs;
    Vector<ExpectedPosting> expected_postings;

    u32 term_num = 30000;
    u32 row_num = 200;
    u32 word_num_pre_row = 100;

    GenerateParagraphs(term_num, row_num, word_num_pre_row, paragraphs, expected_postings);
    const String index_dir = GetFullTmpDir();
    const String dst_base_name = "merged_index";

    u32 base_num = 10;
    u32 row_num_per_base = row_num / base_num;
    Vector<String> base_names;
    Vector<RowID> base_row_ids;
    Vector<u32> row_offsets;
    Vector<u32> row_counts;

    for (u32 i = 0; i < base_num; ++i) {
        base_names.emplace_back(String("chunk") + std::to_string(i + 1));
        base_row_ids.emplace_back(RowID{0U, i * row_num_per_base});
        row_offsets.emplace_back(i * row_num_per_base);
        if (i == base_num - 1) {
            row_counts.emplace_back(row_num - i * row_num_per_base);
        } else {
            row_counts.emplace_back(row_num_per_base);
        }
    }

    u32 random_shuffle_num = 100;
    for (u32 i = 0; i < random_shuffle_num; ++i) {
        u32 idx1 = random() % base_num;
        u32 idx2 = random() % base_num;
        std::swap(base_names[idx1], base_names[idx2]);
        std::swap(base_row_ids[idx1], base_row_ids[idx2]);
        std::swap(row_offsets[idx1], row_offsets[idx2]);
        std::swap(row_counts[idx1], row_counts[idx2]);
    }

    CreateIndex(paragraphs, index_dir, base_names, base_row_ids, row_offsets, row_counts);
    MergeAndCheckIndex(index_dir, base_names, base_row_ids, dst_base_name, expected_postings);
}
#endif
