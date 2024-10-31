#include "gtest/gtest.h"
import base_test;

import stl;
import logical_type;
import third_party;
import infinity_context;
import storage;
import txn_manager;
import table_def;
import column_def;
import data_type;
import extra_ddl_info;
import create_index_info;
import index_full_text;
import index_base;
import block_index;
import base_table_ref;
import catalog;
import segment_entry;
import block_entry;
import column_vector;
import query_builder;
import query_node;
import doc_iterator;
import analyzer;
import term;
import search_driver;
import analyzer_pool;
import index_defines;
import logical_type;
import status;
import txn;
import infinity_exception;
import match_expr;
import internal_types;
import search_options;
import phrase_doc_iterator;
import global_resource_usage;
import term_doc_iterator;
import logger;
import column_index_reader;
import parse_fulltext_options;

using namespace infinity;

class QueryMatchTest : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        InitData();
    }

    void CreateDBAndTable(const String &db_name, const String &table_name);

    void CreateIndex(const String &db_name, const String &table_name, const String &index_name, const String &analyzer);

    void InsertData(const String &db_name, const String &table_name);

    void QueryMatch(const String &db_name,
                    const String &table_name,
                    const String &index_name,
                    const String &fields,
                    const String &match_text,
                    const u32 &expected_doc_freq,
                    const float &expected_matched_freq,
                    const DocIteratorType &query_type);

    void InitData();

public:
    const String data_path_ = "/var/infinity";
    const String db_name_ = "default_db";
    const String table_name_ = "test_table";
    const String index_name_ = "test_fulltext_index";
    String config_path_{};
    TxnTimeStamp last_commit_ts_ = 0;
    Vector<Vector<String>> datas_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         QueryMatchTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

void QueryMatchTest::InitData() {
    datas_ = {
        {"1",
         "Animalia (book)",
         R"#(Animalia is an illustrated children's book by anarchism Graeme Base. It was originally published in 1986, followed by a tenth anniversary edition in 1996, and a 25th anniversary edition in 2012. Over three million copies have been sold. A special numbered and signed anniversary edition was also published in 1996, with an embossed gold jacket.)#"},
        {"2",
         "Academy Award for Best Production Design",
         R"#(The Academy Awards are the oldest awards ceremony for achievements in motion pictures. one of The add test Academy Award for Best Production Design recognizes achievement in art direction on a film. The category's original name was Best Art Direction, but was changed to its current name in 2012 for the 85th Academy Awards.  This change resulted from the Art Director's branch of the Academy being renamed the Designer's branch.)#"},
        {"3",
         "Animation",
         R"#(The American Football Conference (AFC) harm chemical anarchism add test is one of harm chemical the two conferences of the National Football League (NFL). This add test conference and its counterpart, the National Football Conference (NFC), currently contain 16 teams each, making up the 32 teams of the NFL. The current AFC title holder is the New England Patriots.)#"},
        {"4",
         "Foobar",
         R"#(周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。)#"},
    };
}

TEST_P(QueryMatchTest, basic_term) {
    CreateDBAndTable(db_name_, table_name_);
    CreateIndex(db_name_, table_name_, index_name_, "standard");
    InsertData(db_name_, table_name_);
    String fields = "text";
    Vector<String> terms = {"the", "harmful", "chemical", "anarchism"};
    Vector<u32> expected_doc_freq = {2, 1, 1, 2};
    Vector<float> expected_term_freq = {16.0F, 2.0F, 2.0F, 2.0F};
    EXPECT_EQ(terms.size(), expected_doc_freq.size());
    EXPECT_EQ(terms.size(), expected_term_freq.size());
    for (SizeT i = 0; i < terms.size(); ++i) {
        auto term = terms[i];
        auto doc_freq = expected_doc_freq[i];
        auto term_freq = expected_term_freq[i];
        QueryMatch(db_name_, table_name_, index_name_, fields, term, doc_freq, term_freq, DocIteratorType::kTermDocIterator);
    }
}

TEST_P(QueryMatchTest, phrase) {
    CreateDBAndTable(db_name_, table_name_);
    Vector<String> analyzers = {String("standard")};
    for (auto &analyzer : analyzers) {
        CreateIndex(db_name_, table_name_, index_name_, analyzer);
        InsertData(db_name_, table_name_);
        String fields = "text";
        Vector<String> phrases = {R"#("Animalia is an")#",
                                  R"#("one of")#",
                                  R"#("are book")#",
                                  R"#("add test")#",
                                  R"#("harmful chemical")#",
                                  R"#("chemical harmful"~2)#",
                                  R"#("harmful chemical"~10)#",
                                  R"#("make up team"~2)#"};
        Vector<u32> expected_doc_freq = {1, 2, 0, 2, 1, 1, 1, 1};
        Vector<float> expected_phrase_freq = {
            1.0F,
            2.0F,
            0.0f,
            3.0F,
            2.0F,
            2.0F / 3.0F,
            2.0F + 2.0F / 9.0F,
            1.0F / 3.0F,
        };
        EXPECT_EQ(phrases.size(), expected_doc_freq.size());
        EXPECT_EQ(phrases.size(), expected_phrase_freq.size());
        for (SizeT i = 0; i < phrases.size(); ++i) {
            auto phrase = phrases[i];
            auto doc_freq = expected_doc_freq[i];
            auto phrase_freq = expected_phrase_freq[i];
            QueryMatch(db_name_, table_name_, index_name_, fields, phrase, doc_freq, phrase_freq, DocIteratorType::kPhraseIterator);
        }
    }
}

void QueryMatchTest::CreateDBAndTable(const String &db_name, const String &table_name) {
    Vector<SharedPtr<ColumnDef>> column_defs;
    {
        String col1_name = "id";
        auto col1_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto col1_def = MakeShared<ColumnDef>(0, col1_type, std::move(col1_name), std::set<ConstraintType>());
        column_defs.push_back(col1_def);
    }
    {
        String col2_name = "title";
        auto col2_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto col2_def = MakeShared<ColumnDef>(1, col2_type, std::move(col2_name), std::set<ConstraintType>());
        column_defs.push_back(col2_def);
    }
    {
        String col3_name = "text";
        auto col3_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto col3_def = MakeShared<ColumnDef>(2, col3_type, std::move(col3_name), std::set<ConstraintType>());
        column_defs.push_back(col3_def);
    }
    auto table_def = TableDef::Make(MakeShared<String>(db_name), MakeShared<String>(table_name), MakeShared<String>(), std::move(column_defs));
    Storage *storage = InfinityContext::instance().storage();
    TxnManager *txn_mgr = storage->txn_manager();
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("drop table"));
        txn->DropTableCollectionByName(db_name, table_name, ConflictType::kIgnore);
        last_commit_ts_ = txn_mgr->CommitTxn(txn);
    }
    {
        auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("create table"));
        txn->CreateTable(db_name, table_def, ConflictType::kError);

        auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status.ok());

        last_commit_ts_ = txn_mgr->CommitTxn(txn);
    }
}

void QueryMatchTest::CreateIndex(const String &db_name, const String &table_name, const String &index_name, const String &analyzer) {
    Storage *storage = InfinityContext::instance().storage();

    TxnManager *txn_mgr = storage->txn_manager();

    Vector<String> col_name_list{"text"};
    String index_file_name = index_name + ".json";
    {
        auto *txn_idx = txn_mgr->BeginTxn(MakeUnique<String>("create index"));
        auto status0 = txn_idx->DropIndexByName(db_name, table_name, index_name, ConflictType::kIgnore);
        // EXPECT_TRUE(status0.ok());

        auto [table_entry, status1] = txn_idx->GetTableByName(db_name, table_name);
        EXPECT_TRUE(status1.ok());

        IndexFullText full_idx_base(MakeShared<String>(index_name), MakeShared<String>("test comment"), index_file_name, col_name_list, analyzer);
        SharedPtr<IndexBase> full_idx_base_ptr = MakeShared<IndexFullText>(full_idx_base);

        auto [table_idx_entry, status2] = txn_idx->CreateIndexDef(table_entry, full_idx_base_ptr, ConflictType::kInvalid);
        EXPECT_TRUE(status2.ok());
        {
            auto [table_entry, status3] = txn_idx->GetTableByName(db_name, table_name);
            EXPECT_TRUE(status3.ok());

            auto [t_info, status4] = txn_idx->GetTableInfo(db_name, table_name);
            EXPECT_TRUE(status4.ok());
            auto col_cnt = (*t_info).column_count_;

            String alias = "tb1";
            SharedPtr<Vector<SharedPtr<DataType>>> types_ptr = MakeShared<Vector<SharedPtr<DataType>>>();
            SharedPtr<Vector<String>> names_ptr = MakeShared<Vector<String>>();
            Vector<SizeT> columns;

            for (i64 idx = 0; idx < col_cnt; idx++) {
                const ColumnDef *column_def = table_entry->GetColumnDefByIdx(idx);
                types_ptr->emplace_back(column_def->column_type_);
                names_ptr->emplace_back(column_def->name_);
                columns.emplace_back(idx);
            }

            SharedPtr<BlockIndex> block_index = table_entry->GetBlockIndex(txn_idx);

            u64 table_idx = 0;
            auto table_ref = MakeShared<BaseTableRef>(table_entry, std::move(columns), block_index, alias, table_idx, names_ptr, types_ptr);

            auto [_, status5] = txn_idx->CreateIndexPrepare(table_idx_entry, table_ref.get(), true, true);
            EXPECT_TRUE(status5.ok());

            {
                HashMap<SegmentID, atomic_u64> create_index_idxes;
                create_index_idxes[0] = 1;

                auto status6 = txn_idx->CreateIndexDo(table_ref.get(), index_name, create_index_idxes);
                EXPECT_TRUE(status6.ok());
            }

            {
                auto [table_entry, status7] = txn_idx->GetTableByName(db_name, table_name);
                EXPECT_TRUE(status7.ok());

                auto status8 = txn_idx->CreateIndexFinish(table_entry, table_idx_entry);
                EXPECT_TRUE(status8.ok());
            }

            last_commit_ts_ = txn_mgr->CommitTxn(txn_idx);
        }
    }
}

void QueryMatchTest::InsertData(const String &db_name, const String &table_name) {
    Storage *storage = InfinityContext::instance().storage();
    TxnManager *txn_mgr = storage->txn_manager();

    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("import data"));
    auto [table_entry, status] = txn->GetTableByName(db_name, table_name);
    EXPECT_TRUE(status.ok());

    SegmentID segment_id = Catalog::GetNextSegmentID(table_entry);
    SharedPtr<SegmentEntry> segment_entry = SegmentEntry::NewSegmentEntry(table_entry, segment_id, txn);
    {
        for (BlockID block_id = 0; block_id < datas_.size(); ++block_id) {
            UniquePtr<BlockEntry> block_entry =
                BlockEntry::NewBlockEntry(segment_entry.get(), block_id, 0 /*checkpoint_ts*/, table_entry->ColumnCount(), txn);
            {
                Vector<ColumnVector> column_vectors;
                for (SizeT i = 0; i < table_entry->ColumnCount(); ++i) {
                    auto *block_column_entry = block_entry->GetColumnBlockEntry(i);
                    column_vectors.emplace_back(block_column_entry->GetColumnVector(txn->buffer_mgr()));
                }
                auto &row = datas_[block_id];
                for (SizeT i = 0; i < column_vectors.size(); ++i) {
                    auto &column = row[i];
                    column_vectors[i].AppendByStringView(column);
                }
                block_entry->IncreaseRowCount(1);
            }
            segment_entry->AppendBlockEntry(std::move(block_entry));
        }
    }
    segment_entry->FlushNewData();
    txn->Import(table_entry, segment_entry);

    last_commit_ts_ = txn_mgr->CommitTxn(txn);
}

void QueryMatchTest::QueryMatch(const String &db_name,
                                const String &table_name,
                                const String &index_name,
                                const String &fields,
                                const String &match_text,
                                const u32 &expected_doc_freq,
                                const float &expected_matched_freq,
                                const DocIteratorType &query_type) {
    Storage *storage = InfinityContext::instance().storage();
    TxnManager *txn_mgr = storage->txn_manager();

    auto *txn = txn_mgr->BeginTxn(MakeUnique<String>("query match"));

    auto [table_entry, status_table] = txn->GetTableByName(db_name, table_name);
    EXPECT_TRUE(status_table.ok());

    auto [table_index_entry, status_index] = txn->GetIndexByName(db_name, table_name, index_name);
    EXPECT_TRUE(status_index.ok());

    auto fake_table_ref = BaseTableRef::FakeTableRef(table_entry, txn);

    QueryBuilder query_builder(fake_table_ref.get());
    IndexReader index_reader = fake_table_ref->table_entry_ptr_->GetFullTextIndexReader(txn);
    query_builder.Init(index_reader);
    const Map<String, String> &column2analyzer = query_builder.GetColumn2Analyzer();

    auto match_expr = MakeShared<MatchExpr>();
    match_expr->fields_ = fields;
    match_expr->matching_text_ = match_text;

    SearchOptions search_ops(match_expr->options_text_);
    const String &default_field = search_ops.options_["default_field"];

    SearchDriver driver(column2analyzer, default_field);

    UniquePtr<QueryNode> query_tree = driver.ParseSingleWithFields(match_expr->fields_, match_expr->matching_text_);
    if (!query_tree) {
        Status status = Status::ParseMatchExprFailed(match_expr->fields_, match_expr->matching_text_);
        RecoverableError(status);
    }
    FullTextQueryContext full_text_query_context;
    full_text_query_context.query_tree_ = std::move(query_tree);
    UniquePtr<DocIterator> doc_iterator = query_builder.CreateSearch(full_text_query_context, EarlyTermAlgo::kNaive, MinimumShouldMatchOption{});

    RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : (doc_iterator->Next(), doc_iterator->DocID());
    if (iter_row_id == INVALID_ROWID) {
        fmt::print("iter_row_id is INVALID_ROWID\n");
    } else {
        do {
            auto score = doc_iterator->BM25Score();
            fmt::print("iter_row_id = {}, score = {}\n", iter_row_id.ToUint64(), score);
            doc_iterator->Next();
            iter_row_id = doc_iterator->DocID();
        } while (iter_row_id != INVALID_ROWID);
        if (query_type == DocIteratorType::kPhraseIterator) {
            EXPECT_EQ(doc_iterator->GetType(), DocIteratorType::kPhraseIterator);
            auto phrase_iterator = dynamic_cast<PhraseDocIterator *>(doc_iterator.get());
            auto res_df = phrase_iterator->GetDocFreq();
            auto res_phrase_freq = phrase_iterator->GetPhraseFreq();
            EXPECT_EQ(res_df, expected_doc_freq);
            EXPECT_FLOAT_EQ(res_phrase_freq, expected_matched_freq);
        } else {
            EXPECT_EQ(doc_iterator->GetType(), DocIteratorType::kTermDocIterator);
            auto term_iterator = dynamic_cast<TermDocIterator *>(doc_iterator.get());
            auto res_df = term_iterator->GetDocFreq();
            auto res_term_freq = term_iterator->GetTermFreq();
            EXPECT_EQ(res_df, expected_doc_freq);
            EXPECT_FLOAT_EQ(res_term_freq, expected_matched_freq);
        }
    }
    last_commit_ts_ = txn_mgr->CommitTxn(txn);
}
