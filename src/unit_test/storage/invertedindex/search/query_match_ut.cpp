module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.query_match;

import :ut.base_test;

import third_party;
import :infinity_context;
import :storage;
import :table_def;
import :index_full_text;
import :index_base;
import :block_index;
import :base_table_ref;
import :column_vector;
import :query_builder;
import :query_node;
import :doc_iterator;
import :analyzer;
import :term;
import :search_driver;
import :analyzer_pool;
import :index_defines;
import :status;
import :infinity_exception;
import :phrase_doc_iterator;
import :term_doc_iterator;
import :logger;
import :column_index_reader;
import :parse_fulltext_options;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :data_block;

import logical_type;
import column_def;
import data_type;
import extra_ddl_info;
import create_index_info;
import logical_type;
import match_expr;
import internal_types;
import search_options;
import global_resource_usage;

using namespace infinity;

class QueryMatchTest : public BaseTestParamStr {
protected:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        InitData();
    }

    void CreateDBAndTable(const std::string &db_name, const std::string &table_name);

    void CreateIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name, const std::string &analyzer);

    void InsertData(const std::string &db_name, const std::string &table_name);

    void QueryMatch(const std::string &db_name,
                    const std::string &table_name,
                    const std::string &index_name,
                    const std::string &fields,
                    const std::string &match_text,
                    const u32 &expected_doc_freq,
                    const float &expected_matched_freq,
                    const DocIteratorType &query_type);

    void InitData();

public:
    const std::string data_path_ = "/var/infinity";
    const std::string db_name_ = "default_db";
    const std::string table_name_ = "test_table";
    const std::string index_name_ = "test_fulltext_index";
    std::string config_path_{};
    std::vector<std::vector<std::string>> datas_;
    std::shared_ptr<TableDef> table_def_;
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         QueryMatchTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

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
    std::string fields = "text";
    std::vector<std::string> terms = {"the", "harmful", "chemical", "anarchism"};
    std::vector<u32> expected_doc_freq = {2, 1, 1, 2};
    std::vector<float> expected_term_freq = {16.0F, 2.0F, 2.0F, 2.0F};
    EXPECT_EQ(terms.size(), expected_doc_freq.size());
    EXPECT_EQ(terms.size(), expected_term_freq.size());
    for (size_t i = 0; i < terms.size(); ++i) {
        auto term = terms[i];
        auto doc_freq = expected_doc_freq[i];
        auto term_freq = expected_term_freq[i];
        QueryMatch(db_name_, table_name_, index_name_, fields, term, doc_freq, term_freq, DocIteratorType::kTermDocIterator);
    }
}

TEST_P(QueryMatchTest, phrase) {
    CreateDBAndTable(db_name_, table_name_);
    std::vector<std::string> analyzers = {std::string("standard")};
    for (auto &analyzer : analyzers) {
        CreateIndex(db_name_, table_name_, index_name_, analyzer);
        InsertData(db_name_, table_name_);
        std::string fields = "text";
        std::vector<std::string> phrases = {R"#("Animalia is an")#",
                                            R"#("one of")#",
                                            R"#("are book")#",
                                            R"#("add test")#",
                                            R"#("harmful chemical")#",
                                            R"#("chemical harmful"~2)#",
                                            R"#("harmful chemical"~10)#",
                                            R"#("make up team"~2)#"};
        std::vector<u32> expected_doc_freq = {1, 2, 0, 2, 1, 1, 1, 1};
        std::vector<float> expected_phrase_freq = {
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
        for (size_t i = 0; i < phrases.size(); ++i) {
            auto phrase = phrases[i];
            auto doc_freq = expected_doc_freq[i];
            auto phrase_freq = expected_phrase_freq[i];
            QueryMatch(db_name_, table_name_, index_name_, fields, phrase, doc_freq, phrase_freq, DocIteratorType::kPhraseIterator);
        }
    }
}

void QueryMatchTest::CreateDBAndTable(const std::string &db_name, const std::string &table_name) {
    std::vector<std::shared_ptr<ColumnDef>> column_defs;
    {
        std::string col1_name = "id";
        auto col1_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col1_def = std::make_shared<ColumnDef>(0, col1_type, std::move(col1_name), std::set<ConstraintType>());
        column_defs.push_back(col1_def);
    }
    {
        std::string col2_name = "title";
        auto col2_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col2_def = std::make_shared<ColumnDef>(1, col2_type, std::move(col2_name), std::set<ConstraintType>());
        column_defs.push_back(col2_def);
    }
    {
        std::string col3_name = "text";
        auto col3_type = std::make_shared<DataType>(LogicalType::kVarchar);
        auto col3_def = std::make_shared<ColumnDef>(2, col3_type, std::move(col3_name), std::set<ConstraintType>());
        column_defs.push_back(col3_def);
    }
    auto table_def = TableDef::Make(std::make_shared<std::string>(db_name),
                                    std::make_shared<std::string>(table_name),
                                    std::make_shared<std::string>(),
                                    std::move(column_defs));
    NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        txn->DropTable(db_name, table_name, ConflictType::kIgnore);
        Status status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        txn->CreateTable(db_name, table_def, ConflictType::kError);

        Status status = txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    table_def_ = std::move(table_def);
}

void QueryMatchTest::CreateIndex(const std::string &db_name,
                                 const std::string &table_name,
                                 const std::string &index_name,
                                 const std::string &analyzer) {
    Storage *storage = InfinityContext::instance().storage();

    NewTxnManager *txn_mgr = storage->new_txn_manager();
    auto *txn_idx = txn_mgr->BeginTxn(std::make_unique<std::string>("get db"), TransactionType::kRead);
    Status status = txn_idx->DropIndexByName(db_name, table_name, index_name, ConflictType::kIgnore);
    // EXPECT_TRUE(status.ok());

    std::vector<std::string> col_name_list{"text"};
    std::string index_file_name = index_name + ".json";
    auto index_def = std::make_shared<IndexFullText>(std::make_shared<std::string>(index_name),
                                                     std::make_shared<std::string>("test comment"),
                                                     index_file_name,
                                                     col_name_list,
                                                     analyzer);
    auto *txn7 = txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
    status = txn7->CreateIndex(db_name, table_name, index_def, ConflictType::kError);
    EXPECT_TRUE(status.ok());
    status = txn_mgr->CommitTxn(txn7);
    EXPECT_TRUE(status.ok());
}

void QueryMatchTest::InsertData(const std::string &db_name, const std::string &table_name) {
    Storage *storage = InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("import data"), TransactionType::kImport);
    auto input_block = std::make_shared<DataBlock>();
    for (size_t column_id = 0; column_id < table_def_->column_count(); ++column_id) {
        auto col1 = ColumnVector::Make(table_def_->columns()[column_id]->type());
        col1->Initialize();
        for (size_t row_id = 0; row_id < datas_.size(); ++row_id) {
            auto &row = datas_[row_id];
            auto &cell = row[column_id];
            col1->AppendByStringView(cell);
        }
        input_block->InsertVector(col1, column_id);
    }
    input_block->Finalize();
    std::vector<std::shared_ptr<DataBlock>> input_blocks{input_block};
    txn->Import(db_name, table_name, input_blocks);

    Status status = txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());
}

void QueryMatchTest::QueryMatch(const std::string &db_name,
                                const std::string &table_name,
                                const std::string &index_name,
                                const std::string &fields,
                                const std::string &match_text,
                                const u32 &expected_doc_freq,
                                const float &expected_matched_freq,
                                const DocIteratorType &query_type) {
    Storage *storage = InfinityContext::instance().storage();
    NewTxnManager *txn_mgr = storage->new_txn_manager();

    auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("query match"), TransactionType::kRead);
    auto [table_info, status] = txn->GetTableInfo(db_name, table_name);
    EXPECT_TRUE(status.ok());

    std::shared_ptr<IndexReader> index_reader;
    status = txn->GetFullTextIndexReader(db_name, table_name, index_reader);
    EXPECT_TRUE(status.ok());

    QueryBuilder query_builder(table_info);
    query_builder.Init(index_reader);
    std::map<std::string, std::map<std::string, std::string>> column2analyzer = query_builder.GetColumn2Analyzer();

    auto match_expr = std::make_shared<MatchExpr>();
    match_expr->fields_ = fields;
    match_expr->matching_text_ = match_text;

    SearchOptions search_ops(match_expr->options_text_);
    const std::string &default_field = search_ops.options_["default_field"];

    SearchDriver driver(std::move(column2analyzer), default_field);

    std::unique_ptr<QueryNode> query_tree = driver.ParseSingleWithFields(match_expr->fields_, match_expr->matching_text_);
    if (!query_tree) {
        Status status = Status::ParseMatchExprFailed(match_expr->fields_, match_expr->matching_text_);
        RecoverableError(status);
    }
    FullTextQueryContext full_text_query_context(FulltextSimilarity::kBM25, BM25Params{}, MinimumShouldMatchOption{}, RankFeaturesOption{}, 10);
    full_text_query_context.early_term_algo_ = EarlyTermAlgo::kNaive;
    full_text_query_context.query_tree_ = std::move(query_tree);
    std::unique_ptr<DocIterator> doc_iterator = query_builder.CreateSearch(full_text_query_context);

    RowID iter_row_id = doc_iterator.get() == nullptr ? INVALID_ROWID : (doc_iterator->Next(), doc_iterator->DocID());
    if (iter_row_id == INVALID_ROWID) {
        fmt::print("iter_row_id is INVALID_ROWID\n");
    } else {
        do {
            auto score = doc_iterator->Score();
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
    status = txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());
}
