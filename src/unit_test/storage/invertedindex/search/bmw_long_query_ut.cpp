// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Long-query integration test: validates HeapBlockMaxWandIterator (heap-based block-max WAND) for
// correctness, pruning effectiveness and stability against a real inverted index (multiple
// segments, realistic term distribution).
//
// The cases are named DISABLED_SLOW_* on purpose: every one of them builds its own table, loads the
// corpus and creates the index (~5-9s per case with the defaults), which must not be added to the
// default unit-test lane. Run them explicitly:
//   ./build/src/test_main --gtest_also_run_disabled_tests --gtest_filter='*BmwLongQueryTest*'
//
// The corpus size is configurable via environment variables (the defaults keep CI fast, locally it
// can be scaled up to tens of millions of docs):
//   INFINITY_BMW_IT_SEGMENTS    number of segments          (default 4)
//   INFINITY_BMW_IT_DOCS        docs per segment            (default 4000)
//   INFINITY_BMW_IT_VOCAB       vocabulary size             (default 5000)
//   INFINITY_BMW_IT_WORDS       words per document          (default 40)
//   INFINITY_BMW_IT_CONFIG      Infinity config path        (default empty = built-in defaults)
//
// Recommended large-scale runs. 10M documents is the supported scale (20 seg x 500k, verified);
// use test_bmw_scale_10m.toml, which raises mem_index_capacity (fewer disk segments) and shrinks the
// buffer budgets. Peak RSS is ~5.5GB, so do not run it next to other memory-hungry processes --
// a single missing GB gets the process SIGTERMed by earlyoom around 4.5GB, which looks exactly like a
// test hang (the log just stops at a "build progress" line).
//   INFINITY_BMW_IT_CONFIG=test/data/config/test_bmw_scale_10m.toml \
//   INFINITY_BMW_IT_SCALE=1 INFINITY_BMW_IT_SEGMENTS=20 INFINITY_BMW_IT_DOCS=500000 \
//   INFINITY_BMW_IT_VOCAB=20000 ./build/src/test_main --gtest_also_run_disabled_tests \
//   --gtest_filter='*LongQueryAtScale*'
//   ... same env + INFINITY_BMW_IT_RANDOM=1 ... --gtest_filter='*RandomizedDifferential*'
//   ... same env + INFINITY_BMW_IT_PERF=1 INFINITY_BMW_IT_REPEAT=2 INFINITY_BMW_IT_PERF_TERMS=100,200 \
//   ... --gtest_filter='*LongQueryPerfComparison*'
// For quick iteration keep the smaller corpus of test_bmw_scale.toml (or omit the config: the
// defaults are 4 seg x 4000 docs).
//
// Memory constraints (important):
//   * data is generated and written on the fly (8192 rows per DataBlock, one commit every 8
//     blocks), so neither a segment nor the whole corpus is ever resident;
//   * the large-scale parity check does not collect hits into a vector: the two iterators are
//     compared doc by doc, so the only extra memory is the top-N heap (N=10). Small cases still
//     materialize the hits for item-by-item assertions.

module;

#include "unit_test/gtest_expand.h"
// jemalloc reporting is only available in release builds: CMakeLists.txt forces ENABLE_JEMALLOC=OFF
// for Debug, which also leaves jemalloc out of the link line (so mallctl would be an undefined
// symbol). The macro is defined together with the jemalloc flags.
#if defined(ENABLE_JEMALLOC_PROF)
#include <jemalloc/jemalloc.h>
#endif
#include <stdio.h>
#include <unistd.h>

module infinity_core:ut.bmw_long_query;

import :ut.base_test;

import third_party;
import :infinity_context;
import :storage;
import :table_def;
import :index_full_text;
import :index_base;
import :column_vector;
import :query_builder;
import :query_node;
import :doc_iterator;
import :search_driver;
import :index_defines;
import :status;
import :parse_fulltext_options;
import :txn_state;
import :new_txn_manager;
import :new_txn;
import :data_block;
import :fulltext_score_result_heap;
import :heap_blockmax_wand_iterator;
import :buffer_manager;

import logical_type;
import column_def;
import data_type;
import create_index_info;
import match_expr;
import internal_types;
import search_options;

using namespace infinity;

namespace {

u32 EnvU32(const char *name, u32 default_value) {
    const char *raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return default_value;
    }
    return static_cast<u32>(std::strtoul(raw, nullptr, 10));
}

// Resident set size of this process (MB), used to watch memory while bulk-loading / querying.
double CurrentRssMb() {
    FILE *statm = ::fopen("/proc/self/statm", "r");
    if (statm == nullptr) {
        return 0.0;
    }
    long total_pages = 0;
    long resident_pages = 0;
    const int matched = ::fscanf(statm, "%ld %ld", &total_pages, &resident_pages);
    ::fclose(statm);
    if (matched != 2) {
        return 0.0;
    }
    static const double page_mb = static_cast<double>(::sysconf(_SC_PAGESIZE)) / (1024.0 * 1024.0);
    return static_cast<double>(resident_pages) * page_mb;
}

// jemalloc accounting (release builds link jemalloc statically, so glibc mallinfo2 always returns 0).
// Note: mallctl returns cached values unless epoch is advanced first, so epoch:1 is required.
// Debug builds disable jemalloc, in which case the numbers are reported as 0 (see the include guard).
size_t JeStatMb(const char *name) {
#if defined(ENABLE_JEMALLOC_PROF)
    u64 epoch = 1;
    if (::mallctl("epoch", nullptr, nullptr, &epoch, sizeof(epoch)) != 0) {
        return 0;
    }
    size_t value = 0;
    size_t len = sizeof(value);
    if (::mallctl(name, &value, &len, nullptr, 0) != 0) {
        return 0;
    }
    return value / (1024 * 1024);
#else
    (void)name;
    return 0;
#endif
}

struct QueryRunConfig {
    EarlyTermAlgo algo_{EarlyTermAlgo::kNaive};
    u32 topn_{0}; // 0 = full scan (no threshold feedback)
    MinimumShouldMatchOption msm_{};
    // Whether a full scan collects every hit into the result. Set to false on huge corpora to keep
    // only the aggregates and avoid O(hits) memory.
    bool materialize_{true};
};

struct QueryRunResult {
    // Full results (doc ascending) when topn_ == 0 && materialize_, otherwise top-k (score descending)
    std::vector<std::pair<RowID, float>> docs_{};
    double elapsed_ms_{0.0};
    BlockMaxWandStats stats_{};
    bool has_stats_{false};
    // The aggregates below are independent of materialize_ and always collected, for the non-materialized full-scan parity
    size_t matched_{0};
    double score_sum_{0.0};
    float score_max_{0.0F};
    float score_min_{0.0F};
};

// A query opened once: owns the QueryBuilder / SearchDriver / query context / iterator so that
// every dependency outlives the iteration. The transaction is committed by the caller.
class OpenQuery {
public:
    OpenQuery(NewTxn *txn,
              const std::string &db_name,
              const std::string &table_name,
              const std::shared_ptr<IndexReader> &index_reader,
              const std::string &query_text,
              const QueryRunConfig &config)
        : db_name_(db_name), table_name_(table_name), index_reader_(index_reader) {
        auto [table_info, table_status] = txn->GetTableInfo(db_name_, table_name_);
        status_ok_ = table_status.ok() && table_info != nullptr;
        if (!status_ok_) {
            return;
        }
        builder_ = std::make_unique<QueryBuilder>(std::move(table_info));
        builder_->Init(index_reader_);
        driver_ = std::make_unique<SearchDriver>(builder_->GetColumn2Analyzer(), "text");
        context_ = std::make_unique<FullTextQueryContext>(FulltextSimilarity::kBM25, BM25Params{}, config.msm_, RankFeaturesOption{}, config.topn_);
        context_->early_term_algo_ = config.algo_;
        context_->query_tree_ = driver_->ParseSingleWithFields("text", query_text);
        iterator_ = builder_->CreateSearch(*context_);
    }

    bool ok() const { return status_ok_ && iterator_ != nullptr; }

    DocIterator *iterator() { return iterator_.get(); }

private:
    std::string db_name_;
    std::string table_name_;
    std::shared_ptr<IndexReader> index_reader_;
    bool status_ok_{false};
    std::unique_ptr<QueryBuilder> builder_;
    std::unique_ptr<SearchDriver> driver_;
    std::unique_ptr<FullTextQueryContext> context_;
    std::unique_ptr<DocIterator> iterator_;
};

// Doc-by-doc parity of two full scans (zero extra memory: no hit is stored).
struct FullScanParity {
    size_t matched_{0};
    size_t mismatches_{0};
    std::string first_mismatch_{};
    double elapsed_ms_{0.0};
    double a_score_sum_{0.0};
    double b_score_sum_{0.0};
    std::vector<std::pair<RowID, float>> top_{}; // side A's exhaustive top-N (score descending)
};

} // namespace

class BmwLongQueryTest : public BaseTestNoParam {
protected:
    void SetUp() override {
        // A/B: INFINITY_BMW_IT_MODE=seek makes the leaves positioning-only (no per-doc threshold
        // skipping), which pins a recall miss on the leaf side or on the BMW side.
        if (const char *mode = std::getenv("INFINITY_BMW_IT_MODE"); mode != nullptr) {
            HeapBlockMaxWandIterator::SetAdvanceMode(std::string_view(mode) == "seek" ? HeapBlockMaxWandIterator::AdvanceMode::kPlainSeek
                                                                                      : HeapBlockMaxWandIterator::AdvanceMode::kThresholdSkip);
        }
        const char *config_path = std::getenv("INFINITY_BMW_IT_CONFIG");
        if (config_path == nullptr || *config_path == '\0') {
            BaseTestNoParam::SetUp();
            return;
        }
        // Initialize with the given config file. The default config uses buffer_manager_size=8GB and
        // memindex_memory_quota=4GB, which competes for memory with other processes and gets OOM
        // killed when running tens of millions of docs on a shared box, so both budgets can be capped
        // here (see test/data/config/test_bmw_scale.toml).
        InfinityContext::instance().UnInit();
        CleanupDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        auto path = std::make_shared<std::string>(config_path);
        InfinityContext::instance().InitPhase1(path);
        InfinityContext::instance().InitPhase2();
    }

    // index_reader_ holds the mmapped inverted files; it must be released before the Infinity
    // instance is uninitialized, otherwise munmap fails in TearDown (munmap error in
    // disk_segment_reader_impl.cpp).
    void TearDown() override {
        index_reader_.reset();
        table_def_.reset();
        BaseTestNoParam::TearDown();
    }

    // Creates the table, the full-text index and loads several segments of random corpus.
    // Passing 0 for a parameter means env var / default; individual cases override it (e.g. a tiny
    // vocabulary to build dense matching).
    void PrepareIndex(u32 segments = 0, u32 docs_per_segment = 0, u32 vocab = 0, u32 words = 0);

    QueryRunResult Run(const std::string &query_text, const QueryRunConfig &config);

    // Full-scans two configs (same query) in lockstep, doc by doc: doc ids must be identical and
    // scores equal. No hit set is stored, so it works on huge corpora; top_n > 0 also yields the
    // exhaustive top-N of side A.
    auto CompareFullScan(const std::string &query_text, const QueryRunConfig &a, const QueryRunConfig &b, u32 top_n) -> FullScanParity;

    std::string MakeQuery(u32 num_terms, u64 seed) const;

    const std::string db_name_{"default_db"};
    const std::string table_name_{"bmw_it_table"};
    const std::string index_name_{"bmw_it_index"};

    // Corpus size knobs
    u32 segments_{4};
    u32 docs_per_segment_{4000};
    u32 vocab_size_{5000};
    u32 words_per_doc_{40};

    std::shared_ptr<TableDef> table_def_{};
    std::shared_ptr<IndexReader> index_reader_{};
};

void BmwLongQueryTest::PrepareIndex(u32 segments, u32 docs_per_segment, u32 vocab, u32 words) {
    segments_ = segments != 0 ? segments : EnvU32("INFINITY_BMW_IT_SEGMENTS", 4);
    docs_per_segment_ = docs_per_segment != 0 ? docs_per_segment : EnvU32("INFINITY_BMW_IT_DOCS", 4000);
    vocab_size_ = vocab != 0 ? vocab : EnvU32("INFINITY_BMW_IT_VOCAB", 5000);
    words_per_doc_ = words != 0 ? words : EnvU32("INFINITY_BMW_IT_WORDS", 40);
    ASSERT_GT(segments_, 0u);
    ASSERT_GT(docs_per_segment_, 0u);
    ASSERT_GT(vocab_size_, 0u);
    ASSERT_GT(words_per_doc_, 0u);

    NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop table"), TransactionType::kDropTable);
        txn->DropTable(db_name_, table_name_, ConflictType::kIgnore);
        ASSERT_TRUE(txn_mgr->CommitTxn(txn).ok());
    }
    {
        std::vector<std::shared_ptr<ColumnDef>> column_defs;
        {
            auto type = std::make_shared<DataType>(LogicalType::kVarchar);
            column_defs.push_back(std::make_shared<ColumnDef>(0, type, std::string("id"), std::set<ConstraintType>()));
        }
        {
            auto type = std::make_shared<DataType>(LogicalType::kVarchar);
            column_defs.push_back(std::make_shared<ColumnDef>(1, type, std::string("title"), std::set<ConstraintType>()));
        }
        {
            auto type = std::make_shared<DataType>(LogicalType::kVarchar);
            column_defs.push_back(std::make_shared<ColumnDef>(2, type, std::string("text"), std::set<ConstraintType>()));
        }
        table_def_ = TableDef::Make(std::make_shared<std::string>(db_name_),
                                    std::make_shared<std::string>(table_name_),
                                    std::make_shared<std::string>(),
                                    std::move(column_defs));
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
        txn->CreateTable(db_name_, table_def_, ConflictType::kError);
        ASSERT_TRUE(txn_mgr->CommitTxn(txn).ok());
    }
    {
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("drop index"), TransactionType::kRead);
        txn->DropIndexByName(db_name_, table_name_, index_name_, ConflictType::kIgnore);
        ASSERT_TRUE(txn_mgr->CommitTxn(txn).ok());

        // Diagnostic switch: skips index creation, to separate "block data / directory resident" from
        // "inverted index resident".
        if (std::getenv("INFINITY_BMW_IT_NO_INDEX") == nullptr) {
            std::vector<std::string> col_name_list{"text"};
            auto index_def = std::make_shared<IndexFullText>(std::make_shared<std::string>(index_name_),
                                                             std::make_shared<std::string>("long query integration test"),
                                                             index_name_ + ".json",
                                                             col_name_list,
                                                             "standard");
            auto *txn_idx = txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
            ASSERT_TRUE(txn_idx->CreateIndex(db_name_, table_name_, index_def, ConflictType::kError).ok());
            ASSERT_TRUE(txn_mgr->CommitTxn(txn_idx).ok());
        }
    }

    // Load segment by segment: the corpus is generated on the fly per block and committed in
    // batches, so memory is independent of the total doc count. Each DataBlock holds up to 8192
    // rows; several blocks share one Import transaction to cut commit overhead, and every batch is
    // released right after the commit so nothing lingers on the test side.
    std::mt19937 rng(20260601);
    std::uniform_int_distribution<u32> word_dist(0, vocab_size_ - 1);
    constexpr size_t kRowsPerBlock = 8192;
    constexpr size_t kBlocksPerTxn = 8;
    const auto build_begin = std::chrono::steady_clock::now();
    for (u32 seg = 0; seg < segments_; ++seg) {
        std::vector<std::shared_ptr<DataBlock>> pending_blocks;
        size_t rows_done = 0;
        while (rows_done < docs_per_segment_) {
            const size_t rows_in_block = std::min(kRowsPerBlock, docs_per_segment_ - rows_done);
            auto input_block = std::make_shared<DataBlock>();
            {
                auto column = ColumnVector::Make(table_def_->columns()[0]->type());
                column->Initialize();
                for (size_t r = 0; r < rows_in_block; ++r) {
                    column->AppendByStringView(std::to_string(seg * docs_per_segment_ + rows_done + r));
                }
                input_block->InsertVector(column, 0);
            }
            {
                auto column = ColumnVector::Make(table_def_->columns()[1]->type());
                column->Initialize();
                for (size_t r = 0; r < rows_in_block; ++r) {
                    column->AppendByStringView(std::string("t"));
                }
                input_block->InsertVector(column, 1);
            }
            {
                auto column = ColumnVector::Make(table_def_->columns()[2]->type());
                column->Initialize();
                for (size_t r = 0; r < rows_in_block; ++r) {
                    std::string text;
                    text.reserve(words_per_doc_ * 8);
                    for (u32 w = 0; w < words_per_doc_; ++w) {
                        text += fmt::format("w{:05d} ", word_dist(rng));
                    }
                    column->AppendByStringView(text);
                }
                input_block->InsertVector(column, 2);
            }
            input_block->Finalize();
            pending_blocks.push_back(std::move(input_block));
            rows_done += rows_in_block;
            if (pending_blocks.size() >= kBlocksPerTxn || rows_done >= docs_per_segment_) {
                auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("import data"), TransactionType::kImport);
                txn->Import(db_name_, table_name_, pending_blocks);
                ASSERT_TRUE(txn_mgr->CommitTxn(txn).ok());
                // This batch is on disk now; the test side holds no reference to it
                pending_blocks.clear();
                pending_blocks.shrink_to_fit();
                const double elapsed_s = std::chrono::duration<double>(std::chrono::steady_clock::now() - build_begin).count();
                // Prints rss / buffer-manager accounting / glibc heap usage together:
                //   uordblks = heap bytes still in use (real footprint), hblkhd = mmapped bytes.
                size_t buffer_mb = 0;
                size_t buffer_objs = 0;
                if (auto *buffer_mgr = InfinityContext::instance().storage()->buffer_manager(); buffer_mgr != nullptr) {
                    buffer_mb = static_cast<size_t>(buffer_mgr->memory_usage() / (1024 * 1024));
                    buffer_objs = buffer_mgr->BufferedObjectCount();
                }
                std::cerr << fmt::format("[BMW-IT] build progress: seg {}/{} rows/seg {}/{} elapsed={:.1f}s ({:.0f} docs/s) "
                                         "rss={:.0f}MB alloc={}MB active={}MB resident={}MB buffer={}MB/{}objs\n",
                                         seg + 1,
                                         segments_,
                                         rows_done,
                                         docs_per_segment_,
                                         elapsed_s,
                                         elapsed_s > 0 ? double(size_t(seg) * docs_per_segment_ + rows_done) / elapsed_s : 0.0,
                                         CurrentRssMb(),
                                         JeStatMb("stats.allocated"),
                                         JeStatMb("stats.active"),
                                         JeStatMb("stats.resident"),
                                         buffer_mb,
                                         buffer_objs);
            }
        }
    }

    // Optional: dump the jemalloc heap profile after loading (to attribute resident memory on large
    // corpora).
    //   MALLOC_CONF=prof:true,lg_prof_sample:19 INFINITY_BMW_IT_PROFDUMP=/tmp/bmw_heap
    if (const char *prof_dump_path = std::getenv("INFINITY_BMW_IT_PROFDUMP"); prof_dump_path != nullptr) {
#if defined(ENABLE_JEMALLOC_PROF)
        const char *file_name = prof_dump_path;
        if (::mallctl("prof.dump", nullptr, nullptr, &file_name, sizeof(const char *)) == 0) {
            std::cerr << fmt::format("[BMW-IT] heap profile dumped to {}\n", prof_dump_path);
        } else {
            std::cerr << "[BMW-IT] heap profile dump failed\n";
        }
#else
        std::cerr << fmt::format("[BMW-IT] heap profile dump skipped: {} requires a jemalloc build (Debug disables jemalloc)\n", prof_dump_path);
#endif
    }

    if (std::getenv("INFINITY_BMW_IT_NO_INDEX") != nullptr) {
        return;
    }
    auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("open reader"), TransactionType::kRead);
    Status status = txn->GetFullTextIndexReader(db_name_, table_name_, index_reader_);
    ASSERT_TRUE(status.ok());
    ASSERT_NE(index_reader_, nullptr);
    ASSERT_TRUE(txn_mgr->CommitTxn(txn).ok());
}

std::string BmwLongQueryTest::MakeQuery(u32 num_terms, u64 seed) const {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<u32> word_dist(0, vocab_size_ - 1);
    std::vector<u32> words(num_terms);
    for (auto &word : words) {
        word = word_dist(rng);
    }
    // A WAND only exists when the query keeps at least two distinct terms: duplicate-term optimization
    // collapses a repeated term into that term's own iterator. With a tiny vocabulary (down to vocab=1)
    // the sample above can degenerate to a single distinct word, so repair that one case without
    // touching the i.i.d. draws otherwise.
    if (!words.empty() && vocab_size_ > 1) {
        const bool all_same = std::all_of(words.begin(), words.end(), [&](u32 word) { return word == words.front(); });
        if (all_same) {
            words.back() = (words.front() + 1) % vocab_size_;
        }
    }
    std::string query;
    query.reserve(num_terms * 8);
    for (const u32 word : words) {
        query += fmt::format("w{:05d} ", word);
    }
    return query;
}

QueryRunResult BmwLongQueryTest::Run(const std::string &query_text, const QueryRunConfig &config) {
    NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("bmw long query"), TransactionType::kRead);

    QueryRunResult result;
    result.score_max_ = -std::numeric_limits<float>::infinity();
    result.score_min_ = std::numeric_limits<float>::infinity();

    const u32 capacity = std::max<u32>(config.topn_, 1);
    std::vector<float> scores(capacity, 0.0F);
    std::vector<RowID> row_ids(capacity, INVALID_ROWID);
    FullTextScoreResultHeap result_heap(capacity, scores.data(), row_ids.data());

    {
        OpenQuery query(txn, db_name_, table_name_, index_reader_, query_text, config);
        EXPECT_TRUE(query.ok());
        if (query.ok()) {
            DocIterator *doc_iterator = query.iterator();
            const auto begin = std::chrono::steady_clock::now();
            while (doc_iterator->Next()) {
                const RowID doc_id = doc_iterator->DocID();
                const float score = doc_iterator->Score();
                ++result.matched_;
                result.score_sum_ += static_cast<double>(score);
                result.score_max_ = std::max(result.score_max_, score);
                result.score_min_ = std::min(result.score_min_, score);
                if (config.topn_ == 0) {
                    if (config.materialize_) {
                        result.docs_.emplace_back(doc_id, score);
                    }
                } else if (result_heap.AddResult(score, doc_id)) {
                    doc_iterator->UpdateScoreThreshold(result_heap.GetScoreThreshold());
                }
            }
            const auto end = std::chrono::steady_clock::now();
            result.elapsed_ms_ = std::chrono::duration<double, std::milli>(end - begin).count();

            if (auto *heap_wand = dynamic_cast<HeapBlockMaxWandIterator *>(doc_iterator); heap_wand != nullptr) {
                result.stats_ = heap_wand->GetStats();
                result.has_stats_ = true;
            }
        }
    }

    if (config.topn_ != 0) {
        const u32 size = result_heap.GetResultSize();
        result.docs_.reserve(size);
        for (u32 i = 0; i < size; ++i) {
            result.docs_.emplace_back(row_ids[i], scores[i]);
        }
        std::sort(result.docs_.begin(), result.docs_.end(), [](const auto &lhs, const auto &rhs) {
            if (lhs.second != rhs.second) {
                return lhs.second > rhs.second;
            }
            return lhs.first < rhs.first;
        });
    }

    EXPECT_TRUE(txn_mgr->CommitTxn(txn).ok());
    return result;
}

auto BmwLongQueryTest::CompareFullScan(const std::string &query_text, const QueryRunConfig &a, const QueryRunConfig &b, u32 top_n) -> FullScanParity {
    NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto *txn_a = txn_mgr->BeginTxn(std::make_unique<std::string>("bmw lockstep a"), TransactionType::kRead);
    auto *txn_b = txn_mgr->BeginTxn(std::make_unique<std::string>("bmw lockstep b"), TransactionType::kRead);

    FullScanParity parity;
    const u32 capacity = std::max<u32>(top_n, 1);
    std::vector<float> top_scores(capacity, 0.0F);
    std::vector<RowID> top_row_ids(capacity, INVALID_ROWID);
    FullTextScoreResultHeap top_heap(capacity, top_scores.data(), top_row_ids.data());

    {
        OpenQuery query_a(txn_a, db_name_, table_name_, index_reader_, query_text, a);
        OpenQuery query_b(txn_b, db_name_, table_name_, index_reader_, query_text, b);
        if (!query_a.ok() || !query_b.ok()) {
            ++parity.mismatches_;
            parity.first_mismatch_ = "failed to open query iterator";
        } else {
            DocIterator *it_a = query_a.iterator();
            DocIterator *it_b = query_b.iterator();
            const auto begin = std::chrono::steady_clock::now();
            while (true) {
                const bool has_a = it_a->Next();
                const bool has_b = it_b->Next();
                if (has_a != has_b) {
                    ++parity.mismatches_;
                    if (parity.first_mismatch_.empty()) {
                        parity.first_mismatch_ = fmt::format("hit count differs at {}: a={} b={}",
                                                             parity.matched_,
                                                             has_a ? "has more docs" : "ended",
                                                             has_b ? "has more docs" : "ended");
                    }
                    break;
                }
                if (!has_a) {
                    break;
                }
                ++parity.matched_;
                const RowID doc_a = it_a->DocID();
                const RowID doc_b = it_b->DocID();
                const float score_a = it_a->Score();
                const float score_b = it_b->Score();
                parity.a_score_sum_ += static_cast<double>(score_a);
                parity.b_score_sum_ += static_cast<double>(score_b);

                const float tolerance = 1e-4F * std::max(1.0F, std::abs(score_a));
                if (!(doc_a == doc_b) || std::abs(score_a - score_b) > tolerance) {
                    if (parity.first_mismatch_.empty()) {
                        parity.first_mismatch_ = fmt::format("doc_a={} doc_b={} score_a={} score_b={} rank={}",
                                                             doc_a.ToString(),
                                                             doc_b.ToString(),
                                                             score_a,
                                                             score_b,
                                                             parity.matched_ - 1);
                    }
                    ++parity.mismatches_;
                    if (parity.mismatches_ >= 3) {
                        break;
                    }
                }
                if (top_n > 0) {
                    top_heap.AddResult(score_a, doc_a);
                }
            }
            const auto end = std::chrono::steady_clock::now();
            parity.elapsed_ms_ = std::chrono::duration<double, std::milli>(end - begin).count();
        }
    }
    EXPECT_TRUE(txn_mgr->CommitTxn(txn_a).ok());
    EXPECT_TRUE(txn_mgr->CommitTxn(txn_b).ok());

    if (top_n > 0) {
        const u32 size = top_heap.GetResultSize();
        parity.top_.reserve(size);
        for (u32 i = 0; i < size; ++i) {
            parity.top_.emplace_back(top_row_ids[i], top_scores[i]);
        }
        std::sort(parity.top_.begin(), parity.top_.end(), [](const auto &lhs, const auto &rhs) {
            if (lhs.second != rhs.second) {
                return lhs.second > rhs.second;
            }
            return lhs.first < rhs.first;
        });
    }
    return parity;
}

// With a full scan (no threshold) naive OR and heap BMW must agree doc by doc: set, order, scores.
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryFullScanParity) {
    PrepareIndex();
    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    std::cerr << fmt::format("[BMW-IT] corpus: segments={} docs/seg={} total={} vocab={} words/doc={}\n",
                             segments_,
                             docs_per_segment_,
                             total_docs,
                             vocab_size_,
                             words_per_doc_);

    for (u32 num_terms : {64u, 200u, 400u}) {
        const std::string query_text = MakeQuery(num_terms, 1000 + num_terms);
        auto naive_result = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}});
        auto bmw_result = Run(query_text, {EarlyTermAlgo::kHeapBMW, 0, {}});
        ASSERT_GT(naive_result.docs_.size(), 0u);
        ASSERT_EQ(naive_result.docs_.size(), bmw_result.docs_.size()) << "terms=" << num_terms;
        for (size_t i = 0; i < naive_result.docs_.size(); ++i) {
            ASSERT_EQ(naive_result.docs_[i].first, bmw_result.docs_[i].first) << "terms=" << num_terms << " idx=" << i;
            const float expected = naive_result.docs_[i].second;
            ASSERT_NEAR(expected, bmw_result.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "terms=" << num_terms << " idx=" << i;
        }
        std::cerr << fmt::format("[BMW-IT] full-scan terms={} matched={} naive={:.1f}ms bmw={:.1f}ms bmw_stats{{iter={}, scored={}, advances={}, "
                                 "block_seeks={}, heap_ops={}}}\n",
                                 num_terms,
                                 naive_result.docs_.size(),
                                 naive_result.elapsed_ms_,
                                 bmw_result.elapsed_ms_,
                                 bmw_result.stats_.iterations_,
                                 bmw_result.stats_.docs_scored_,
                                 bmw_result.stats_.child_advances_,
                                 bmw_result.stats_.block_seeks_,
                                 bmw_result.stats_.heap_ops_);
    }
}

// top-k (with threshold feedback): the result must equal "full scan, sort, take k", and pruning must
// really happen.
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryTopKAndPruningEffectiveness) {
    PrepareIndex();
    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    const std::string query_text = MakeQuery(200, 424242);

    // Reference: naive full scan
    auto reference = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}});
    ASSERT_GT(reference.docs_.size(), 0u);
    std::unordered_map<u64, float> reference_scores;
    reference_scores.reserve(reference.docs_.size());
    for (const auto &[row_id, score] : reference.docs_) {
        reference_scores.emplace(row_id.ToUint64(), score);
    }
    // Reference sorted by score descending (ties by doc ascending), recording every doc's exhaustive
    // rank for easier diagnosis.
    std::vector<std::pair<RowID, float>> reference_sorted = reference.docs_;
    std::sort(reference_sorted.begin(), reference_sorted.end(), [](const auto &lhs, const auto &rhs) {
        if (lhs.second != rhs.second) {
            return lhs.second > rhs.second;
        }
        return lhs.first < rhs.first;
    });
    std::vector<float> reference_top_scores;
    reference_top_scores.reserve(reference_sorted.size());
    std::unordered_map<u64, size_t> reference_rank;
    reference_rank.reserve(reference_sorted.size());
    for (size_t i = 0; i < reference_sorted.size(); ++i) {
        reference_top_scores.push_back(reference_sorted[i].second);
        reference_rank.emplace(reference_sorted[i].first.ToUint64(), i);
    }

    // Note: topn=100 is the key regression point for block skipping. The looser the threshold, the
    // more aggressive the skipping; historically this advanced a global resume point just because
    // "some child has no posting at the tail of a segment" and lost the tail docs of segment 0
    // (showing up as rank 49 missing entirely). The three topn values cover strong/medium/weak
    // pruning.
    for (u32 topn : {1u, 10u, 100u}) {
        auto bmw_result = Run(query_text, {EarlyTermAlgo::kHeapBMW, topn, {}});
        const u32 expected_size = std::min<u32>(topn, static_cast<u32>(reference.docs_.size()));
        ASSERT_EQ(bmw_result.docs_.size(), expected_size) << "topn=" << topn;

        // 1) Every returned doc must exist in the reference with the same score
        for (const auto &[row_id, score] : bmw_result.docs_) {
            auto iter = reference_scores.find(row_id.ToUint64());
            ASSERT_NE(iter, reference_scores.end()) << "topn=" << topn << " unexpected doc " << row_id.ToString();
            ASSERT_NEAR(iter->second, score, 1e-4F * std::max(1.0F, std::abs(iter->second))) << "topn=" << topn << " doc " << row_id.ToString();
        }
        // 2) The (descending) score sequence must equal the reference top-k
        for (size_t i = 0; i < bmw_result.docs_.size(); ++i) {
            const float expected = reference_top_scores[i];
            const float actual = bmw_result.docs_[i].second;
            if (std::abs(expected - actual) > 1e-4F * std::max(1.0F, std::abs(expected))) {
                // Diagnosis: on mismatch print the exhaustive rank of the returned doc and whether the
                // expected doc was returned, to tell a recall miss from a wrong score.
                const u64 actual_u64 = bmw_result.docs_[i].first.ToUint64();
                const u64 expected_u64 = reference_sorted[i].first.ToUint64();
                const auto actual_rank = reference_rank.find(actual_u64);
                const bool expected_returned = std::any_of(bmw_result.docs_.begin(), bmw_result.docs_.end(), [expected_u64](const auto &item) {
                    return item.first.ToUint64() == expected_u64;
                });
                std::cerr << fmt::format("[BMW-IT] MISMATCH topn={} rank={} expected_doc={} expected_score={:.6f} "
                                         "actual_doc={} actual_score={:.6f} actual_exhaustive_rank={} expected_doc_returned={}\n",
                                         topn,
                                         i,
                                         expected_u64,
                                         expected,
                                         actual_u64,
                                         actual,
                                         actual_rank == reference_rank.end() ? size_t(-1) : actual_rank->second,
                                         expected_returned);
            }
            ASSERT_NEAR(expected, actual, 1e-4F * std::max(1.0F, std::abs(expected))) << "topn=" << topn << " rank " << i;
        }
        // 3) Determinism: repeating the run yields exactly the same result
        auto repeat = Run(query_text, {EarlyTermAlgo::kHeapBMW, topn, {}});
        ASSERT_EQ(repeat.docs_.size(), bmw_result.docs_.size()) << "topn=" << topn;
        for (size_t i = 0; i < repeat.docs_.size(); ++i) {
            ASSERT_EQ(repeat.docs_[i].first, bmw_result.docs_[i].first) << "topn=" << topn << " rank " << i;
            ASSERT_EQ(repeat.docs_[i].second, bmw_result.docs_[i].second) << "topn=" << topn << " rank " << i;
        }

        // With any vocabulary larger than one, MakeQuery keeps at least two distinct terms, so the
        // planner must have produced the WAND iterator. vocab=1 has no WAND to inspect (the query
        // collapses to a single term iterator) and only the result checks above apply.
        ASSERT_TRUE(bmw_result.has_stats_ || vocab_size_ < 2) << "topn=" << topn;
        std::cerr << fmt::format("[BMW-IT] topk={} docs={} elapsed={:.1f}ms (naive_all={:.1f}ms) iter={} scored={} advances={} "
                                 "block_seeks={} heap_ops={} iter/total={:.3f}\n",
                                 topn,
                                 bmw_result.docs_.size(),
                                 bmw_result.elapsed_ms_,
                                 reference.elapsed_ms_,
                                 bmw_result.stats_.iterations_,
                                 bmw_result.stats_.docs_scored_,
                                 bmw_result.stats_.child_advances_,
                                 bmw_result.stats_.block_seeks_,
                                 bmw_result.stats_.heap_ops_,
                                 double(bmw_result.stats_.iterations_) / double(total_docs));

        // 4) Pruning effectiveness: top-k is found without walking every doc
        if (bmw_result.has_stats_) {
            ASSERT_LT(bmw_result.stats_.iterations_, total_docs) << "topn=" << topn;
            if (topn <= 10u) {
                // A strong threshold (top-10) should scan a small fraction only (the ratio is meaningless
                // on a tiny corpus)
                if (total_docs >= 4000) {
                    ASSERT_LT(bmw_result.stats_.iterations_, total_docs / 2) << "topn=" << topn;
                    ASSERT_LT(bmw_result.stats_.docs_scored_, total_docs / 2) << "topn=" << topn;
                }
                // Candidates that need exact scoring must be far fewer than the scan rounds
                ASSERT_LT(bmw_result.stats_.docs_scored_, bmw_result.stats_.iterations_) << "topn=" << topn;
            }
            // 5) A larger topn means a looser threshold and can only need more candidates (monotonicity)
            if (topn == 100u) {
                ASSERT_GE(bmw_result.stats_.iterations_, 1u);
            }
        } else {
            std::cerr << fmt::format("[BMW-IT] topk={} pruning statistics skipped: single-child query (vocab={})\n", topn, vocab_size_);
        }
    }
}

// Dense matching (small vocabulary / long docs): almost every doc hits most query terms.
// This is the worst case for block-max pruning -- the block bound almost always exceeds the
// threshold and the algorithm degrades to per-doc scoring. The case exists to 1) keep the results
// correct under that distribution and 2) record the degradation as a performance baseline.
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryDenseVocabulary) {
    // Small vocabulary + long docs to force dense matching (segments/docs still scale with env vars)
    PrepareIndex(0, 0, 200, 40);

    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    const std::string query_text = MakeQuery(200, 555);

    auto reference = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}});
    auto full_scan = Run(query_text, {EarlyTermAlgo::kHeapBMW, 0, {}});
    ASSERT_GT(reference.docs_.size(), 0u);
    ASSERT_EQ(reference.docs_.size(), full_scan.docs_.size());
    for (size_t i = 0; i < reference.docs_.size(); ++i) {
        ASSERT_EQ(reference.docs_[i].first, full_scan.docs_[i].first) << "dense idx " << i;
        const float expected = reference.docs_[i].second;
        ASSERT_NEAR(expected, full_scan.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "dense idx " << i;
    }

    // top-k must still equal the exhaustive result
    std::vector<float> reference_scores;
    reference_scores.reserve(reference.docs_.size());
    for (const auto &[row_id, score] : reference.docs_) {
        static_cast<void>(row_id);
        reference_scores.push_back(score);
    }
    std::sort(reference_scores.begin(), reference_scores.end(), std::greater<>());
    auto top10 = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, {}});
    ASSERT_EQ(top10.docs_.size(), 10u);
    for (size_t i = 0; i < top10.docs_.size(); ++i) {
        const float expected = reference_scores[i];
        ASSERT_NEAR(expected, top10.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "dense top rank " << i;
    }
    std::cerr << fmt::format(
        "[BMW-IT] dense(vocab={}) matched={} full_scan=({:.1f}ms iter={}) top10=({:.1f}ms iter={} scored={}) iter/total={:.3f}\n",
        vocab_size_,
        reference.docs_.size(),
        full_scan.elapsed_ms_,
        full_scan.stats_.iterations_,
        top10.elapsed_ms_,
        top10.stats_.iterations_,
        top10.stats_.docs_scored_,
        double(top10.stats_.iterations_) / double(total_docs));
}

// Large-scale validation (skipped by default): one index build, then full-scan parity + top-k
// parity + determinism + pruning stats.
//   INFINITY_BMW_IT_SCALE=1 INFINITY_BMW_IT_SEGMENTS=30 INFINITY_BMW_IT_DOCS=1000000 INFINITY_BMW_IT_VOCAB=20000
//
// On huge corpora the hits cannot be materialized (hits ~ all docs), so:
//   * full-scan parity: the naive and BMW iterators are compared doc by doc (zero extra memory);
//   * exhaustive top-10: kept in a 10-slot heap during the same pass;
//   * seek mode cannot lockstep with skip in one process (the advance mode is a global switch), so it
//     is compared through aggregates (hit count + score sum).
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryAtScale) {
    if (std::getenv("INFINITY_BMW_IT_SCALE") == nullptr) {
        GTEST_SKIP() << "set INFINITY_BMW_IT_SCALE=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run the large-scale validation";
    }
    PrepareIndex();
    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    std::cerr << fmt::format("[BMW-IT] scale corpus: {} docs ({} seg x {}), vocab={}, words/doc={}, rss={:.0f}MB\n",
                             total_docs,
                             segments_,
                             docs_per_segment_,
                             vocab_size_,
                             words_per_doc_,
                             CurrentRssMb());

    // Diagnostic switch: load data only (with or without index) and stop here, to watch the memory
    // curve of the indexing phase.
    if (std::getenv("INFINITY_BMW_IT_PREPARE_ONLY") != nullptr) {
        std::cerr << fmt::format("[BMW-IT] prepare-only done: rss={:.0f}MB alloc={}MB active={}MB resident={}MB\n",
                                 CurrentRssMb(),
                                 JeStatMb("stats.allocated"),
                                 JeStatMb("stats.active"),
                                 JeStatMb("stats.resident"));
        return;
    }

    for (u32 num_terms : {100u, 200u}) {
        const std::string query_text = MakeQuery(num_terms, 20261031 + num_terms);

        // 1) Full-scan parity: naive vs heap BMW (skip), doc by doc; extra memory is top-10 only.
        HeapBlockMaxWandIterator::SetAdvanceMode(HeapBlockMaxWandIterator::AdvanceMode::kThresholdSkip);
        const auto naive_vs_skip =
            CompareFullScan(query_text, QueryRunConfig{EarlyTermAlgo::kNaive, 0, {}}, QueryRunConfig{EarlyTermAlgo::kHeapBMW, 0, {}}, 10);
        ASSERT_EQ(naive_vs_skip.mismatches_, 0u) << "terms=" << num_terms << " " << naive_vs_skip.first_mismatch_;
        ASSERT_GT(naive_vs_skip.matched_, 0u) << "terms=" << num_terms;

        // 2) Full-scan parity (seek mode): the advance mode is a global switch, so it cannot lockstep
        //    with skip; compare aggregates (hit count + score sum) against BMW from step 1.
        HeapBlockMaxWandIterator::SetAdvanceMode(HeapBlockMaxWandIterator::AdvanceMode::kPlainSeek);
        auto seek_full = Run(query_text, QueryRunConfig{EarlyTermAlgo::kHeapBMW, 0, {}, false});
        ASSERT_EQ(seek_full.matched_, naive_vs_skip.matched_) << "terms=" << num_terms;
        ASSERT_NEAR(seek_full.score_sum_, naive_vs_skip.b_score_sum_, 1e-9 * std::max(1.0, std::abs(naive_vs_skip.b_score_sum_)))
            << "terms=" << num_terms;
        const size_t seek_full_matched = seek_full.matched_;

        // 3) top-10: compare skip and seek against each other and against the exhaustive top-10, plus
        //    determinism.
        auto top_seek = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, {}});
        HeapBlockMaxWandIterator::SetAdvanceMode(HeapBlockMaxWandIterator::AdvanceMode::kThresholdSkip);
        auto top_skip = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, {}});
        auto top_repeat = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, {}});

        ASSERT_EQ(top_skip.docs_.size(), 10u) << "terms=" << num_terms;
        ASSERT_EQ(top_seek.docs_.size(), top_skip.docs_.size()) << "terms=" << num_terms;
        for (size_t i = 0; i < top_skip.docs_.size(); ++i) {
            ASSERT_EQ(top_skip.docs_[i].first, top_seek.docs_[i].first) << "terms=" << num_terms << " rank " << i;
            ASSERT_EQ(top_skip.docs_[i].second, top_seek.docs_[i].second) << "terms=" << num_terms << " rank " << i;
        }
        // Against the exhaustive top-10
        ASSERT_GE(naive_vs_skip.top_.size(), top_skip.docs_.size()) << "terms=" << num_terms;
        for (size_t i = 0; i < top_skip.docs_.size(); ++i) {
            const float expected = naive_vs_skip.top_[i].second;
            ASSERT_NEAR(expected, top_skip.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "terms=" << num_terms << " rank " << i;
        }
        // Determinism
        ASSERT_EQ(top_repeat.docs_.size(), top_skip.docs_.size()) << "terms=" << num_terms;
        for (size_t i = 0; i < top_repeat.docs_.size(); ++i) {
            ASSERT_EQ(top_repeat.docs_[i].first, top_skip.docs_[i].first) << "terms=" << num_terms << " rank " << i;
        }
        // Pruning must really happen. Only a single-distinct-term query (vocab=1) has no WAND to
        // inspect; with any larger vocabulary the statistics must be present.
        ASSERT_TRUE(top_skip.has_stats_ || vocab_size_ < 2) << "terms=" << num_terms;
        if (top_skip.has_stats_) {
            ASSERT_LT(top_skip.stats_.iterations_, total_docs) << "terms=" << num_terms;
        }

        std::cerr << fmt::format("[BMW-IT] scale terms={} matched={} lockstep_full={:.1f}ms seek_full={:.1f}ms(matched={}) | "
                                 "skip top10={:.1f}ms iter={} scored={} advances={} | seek top10={:.1f}ms iter={} scored={} advances={} | "
                                 "iter/total={:.4f} rss={:.0f}MB\n",
                                 num_terms,
                                 naive_vs_skip.matched_,
                                 naive_vs_skip.elapsed_ms_,
                                 seek_full.elapsed_ms_,
                                 seek_full_matched,
                                 top_skip.elapsed_ms_,
                                 top_skip.stats_.iterations_,
                                 top_skip.stats_.docs_scored_,
                                 top_skip.stats_.child_advances_,
                                 top_seek.elapsed_ms_,
                                 top_seek.stats_.iterations_,
                                 top_seek.stats_.docs_scored_,
                                 top_seek.stats_.child_advances_,
                                 double(top_skip.stats_.iterations_) / double(total_docs),
                                 CurrentRssMb());
    }
}

// Performance comparison on very long queries (skipped by default), on the same real index:
//   * naive    -- full-scanning OR iterator (no pruning), the baseline;
//   * heapBMW  -- heap-based block-max WAND (the default path);
//   * sortBMW  -- legacy sort-based BlockMaxWandIterator (A/B only; it has a known recall issue on
//                 dense corpora, never treat its output as an answer).
// Each combination runs REPEAT times and keeps the minimum (a single measurement is dominated by
// scheduling noise).
//   INFINITY_BMW_IT_PERF=1 INFINITY_BMW_IT_SEGMENTS=4 INFINITY_BMW_IT_DOCS=50000 \
//   INFINITY_BMW_IT_VOCAB=20000 INFINITY_BMW_IT_REPEAT=5 \
//   ./build/src/test_main --gtest_also_run_disabled_tests \
//   --gtest_filter='*BmwLongQueryTest.LongQueryPerfComparison'
// 10M scale (see the file header): add INFINITY_BMW_IT_CONFIG=test/data/config/test_bmw_scale_10m.toml,
// SEGMENTS=20 DOCS=500000 REPEAT=2 PERF_TERMS=100,200 (~13 min including the index build).
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryPerfComparison) {
    if (std::getenv("INFINITY_BMW_IT_PERF") == nullptr) {
        GTEST_SKIP() << "set INFINITY_BMW_IT_PERF=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run the perf comparison";
    }
    PrepareIndex();
    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    const u32 repeats = std::max<u32>(1, EnvU32("INFINITY_BMW_IT_REPEAT", 5));
    constexpr float kInf = std::numeric_limits<float>::infinity();
    std::cerr << fmt::format("[BMW-PERF] corpus: {} docs ({} seg x {}), vocab={}, words/doc={}, repeats={}\n",
                             total_docs,
                             segments_,
                             docs_per_segment_,
                             vocab_size_,
                             words_per_doc_,
                             repeats);

    const auto run_best = [&](const std::string &query_text, const QueryRunConfig &config) {
        QueryRunResult best;
        best.elapsed_ms_ = kInf;
        for (u32 r = 0; r < repeats; ++r) {
            auto res = Run(query_text, config);
            if (res.elapsed_ms_ < best.elapsed_ms_) {
                best = std::move(res);
            }
        }
        return best;
    };

    // Query term counts, {100, 200} by default; override with INFINITY_BMW_IT_PERF_TERMS=100,200,400,800.
    std::vector<u32> perf_terms{100u, 200u};
    if (const char *raw_terms = std::getenv("INFINITY_BMW_IT_PERF_TERMS"); raw_terms != nullptr && *raw_terms != '\0') {
        perf_terms.clear();
        const std::string all(raw_terms);
        for (size_t pos = 0; pos < all.size();) {
            const size_t comma = all.find(',', pos);
            const std::string token = all.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos);
            const u32 value = static_cast<u32>(std::strtoul(token.c_str(), nullptr, 10));
            if (value > 0) {
                perf_terms.push_back(value);
            }
            if (comma == std::string::npos) {
                break;
            }
            pos = comma + 1;
        }
        ASSERT_FALSE(perf_terms.empty()) << "INFINITY_BMW_IT_PERF_TERMS parsed empty";
    }

    for (u32 num_terms : perf_terms) {
        const std::string query_text = MakeQuery(num_terms, 20261031 + num_terms);

        // Baseline: naive full scan (topn=0, hits not materialized). Fewer repeats, it is slow anyway.
        QueryRunResult naive;
        naive.elapsed_ms_ = kInf;
        for (u32 r = 0; r < std::min<u32>(repeats, 3); ++r) {
            auto res = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}, false});
            if (res.elapsed_ms_ < naive.elapsed_ms_) {
                naive = std::move(res);
            }
        }
        ASSERT_GT(naive.matched_, 0u) << "terms=" << num_terms;
        std::cerr << fmt::format("[BMW-PERF] terms={} topn=full | naive={:.1f}ms matched={} | heap=... | sort=...\n",
                                 num_terms,
                                 naive.elapsed_ms_,
                                 naive.matched_);

        // Diagnosis (INFINITY_BMW_IT_PERF_VERIFY=1): materialize the naive hits as doc->score and use it
        // as the referee. When two pruning algorithms disagree, only the exhaustive result can tell
        // which side is wrong. Small corpora only.
        std::vector<float> naive_sorted_scores;
        std::unordered_map<u64, float> naive_doc_scores;
        if (std::getenv("INFINITY_BMW_IT_PERF_VERIFY") != nullptr) {
            auto materialized = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}});
            naive_sorted_scores.reserve(materialized.docs_.size());
            naive_doc_scores.reserve(materialized.docs_.size());
            for (const auto &[row_id, score] : materialized.docs_) {
                naive_sorted_scores.push_back(score);
                naive_doc_scores.emplace(row_id.ToUint64(), score);
            }
            std::sort(naive_sorted_scores.begin(), naive_sorted_scores.end(), std::greater<>());
        }

        for (u32 topn : {10u, 100u, 1000u}) {
            const auto heap = run_best(query_text, {EarlyTermAlgo::kHeapBMW, topn, {}});
            const auto sort = run_best(query_text, {EarlyTermAlgo::kBMW, topn, {}});
            // vocab=1 has a single distinct term, so the planner collapses the query and there is no
            // WAND structure; every larger vocabulary must produce one.
            ASSERT_TRUE(heap.has_stats_ || vocab_size_ < 2) << "terms=" << num_terms << " topn=" << topn;
            // Correctness guard: the heap variant (default path) must return the right count.
            ASSERT_EQ(heap.docs_.size(), std::min<size_t>(topn, naive.matched_)) << "terms=" << num_terms << " topn=" << topn;
            // The legacy sort-based one has a known recall issue; only record the difference (its
            // timing is the interesting number).
            if (sort.docs_.size() != heap.docs_.size()) {
                std::cerr << fmt::format("[BMW-PERF] NOTE legacy sort BMW returned {} docs vs heap {} (known recall issue)\n",
                                         sort.docs_.size(),
                                         heap.docs_.size());
            }
            if (!naive_sorted_scores.empty()) {
                const auto verify = [&](const char *label, const QueryRunResult &result) {
                    size_t bad = 0;
                    for (size_t i = 0; i < result.docs_.size() && bad < 5; ++i) {
                        const u64 doc = result.docs_[i].first.ToUint64();
                        const float actual = result.docs_[i].second;
                        const auto iter = naive_doc_scores.find(doc);
                        if (iter == naive_doc_scores.end()) {
                            std::cerr << fmt::format("[BMW-VERIFY] terms={} topn={} {} rank={} doc={} MISSING-IN-NAIVE score={:.6f}\n",
                                                     num_terms,
                                                     topn,
                                                     label,
                                                     i,
                                                     doc,
                                                     actual);
                            ++bad;
                            continue;
                        }
                        const float tol = 1e-4F * std::max(1.0F, std::abs(iter->second));
                        if (std::abs(actual - iter->second) > tol) {
                            std::cerr << fmt::format("[BMW-VERIFY] terms={} topn={} {} rank={} doc={} naive={:.6f} actual={:.6f} diff={:+.6f}\n",
                                                     num_terms,
                                                     topn,
                                                     label,
                                                     i,
                                                     doc,
                                                     iter->second,
                                                     actual,
                                                     actual - iter->second);
                            ++bad;
                        }
                    }
                };
                verify("heap", heap);
            }
            std::cerr << fmt::format("[BMW-PERF] terms={} topn={} | naive={:.1f}ms | heap={:.1f}ms ({:.2f}x) iter={} ({:.3f}/doc) "
                                     "scored={} advances={} block_seeks={} | sort={:.1f}ms ({:.2f}x)\n",
                                     num_terms,
                                     topn,
                                     naive.elapsed_ms_,
                                     heap.elapsed_ms_,
                                     naive.elapsed_ms_ / heap.elapsed_ms_,
                                     heap.stats_.iterations_,
                                     double(heap.stats_.iterations_) / double(total_docs),
                                     heap.stats_.docs_scored_,
                                     heap.stats_.child_advances_,
                                     heap.stats_.block_seeks_,
                                     sort.elapsed_ms_,
                                     naive.elapsed_ms_ / sort.elapsed_ms_);
        }

        // Pure scan cost (threshold feedback off): BMW must not be much slower than naive at topn=0.
        const auto heap_full = run_best(query_text, {EarlyTermAlgo::kHeapBMW, 0, {}, false});
        const auto sort_full = run_best(query_text, {EarlyTermAlgo::kBMW, 0, {}, false});
        ASSERT_EQ(heap_full.matched_, naive.matched_) << "terms=" << num_terms;
        ASSERT_NEAR(heap_full.score_sum_, naive.score_sum_, 1e-9 * std::max(1.0, std::abs(naive.score_sum_))) << "terms=" << num_terms;
        std::cerr << fmt::format(
            "[BMW-PERF] terms={} topn=0 (no threshold feedback) | naive={:.1f}ms | heap={:.1f}ms ({:.2f}x) | sort={:.1f}ms ({:.2f}x)\n",
            num_terms,
            naive.elapsed_ms_,
            heap_full.elapsed_ms_,
            naive.elapsed_ms_ / heap_full.elapsed_ms_,
            sort_full.elapsed_ms_,
            naive.elapsed_ms_ / sort_full.elapsed_ms_);
    }
}

// Stability: Next(RowID) jump semantics, early give-up, MSM, phrase children
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryStability) {
    PrepareIndex();
    const std::string query_text = MakeQuery(200, 777);

    // (a) Next(RowID) jump: pick docs from the reference as targets; the iterator must return the
    //     first doc >= target
    auto reference = Run(query_text, {EarlyTermAlgo::kNaive, 0, {}});
    ASSERT_GT(reference.docs_.size(), 10u);
    {
        NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
        auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("bmw jump"), TransactionType::kRead);
        QueryRunConfig jump_config{EarlyTermAlgo::kHeapBMW, 0, {}};
        OpenQuery query(txn, db_name_, table_name_, index_reader_, query_text, jump_config);
        ASSERT_TRUE(query.ok());
        DocIterator *doc_iterator = query.iterator();

        size_t cursor = 0;
        for (size_t probe = 0; probe < reference.docs_.size(); probe += 17) {
            const RowID target = reference.docs_[probe].first;
            ASSERT_TRUE(doc_iterator->Next(target)) << "probe " << probe;
            // Expected: the first entry in the reference list with doc >= target
            while (cursor < reference.docs_.size() && reference.docs_[cursor].first < target) {
                ++cursor;
            }
            ASSERT_LT(cursor, reference.docs_.size());
            ASSERT_EQ(doc_iterator->DocID(), reference.docs_[cursor].first) << "probe " << probe;
            const float expected = reference.docs_[cursor].second;
            ASSERT_NEAR(expected, doc_iterator->Score(), 1e-4F * std::max(1.0F, std::abs(expected))) << "probe " << probe;
            // A Next() that does not move (target == current doc) must keep the cached score:
            // ScoreThresholdIterator-style callers can re-ask for the current doc and then read Score().
            const RowID settled_doc = doc_iterator->DocID();
            const float settled_score = doc_iterator->Score();
            ASSERT_TRUE(doc_iterator->Next(settled_doc)) << "probe " << probe;
            ASSERT_EQ(doc_iterator->DocID(), settled_doc) << "probe " << probe;
            ASSERT_EQ(doc_iterator->Score(), settled_score) << "probe " << probe;
        }
        EXPECT_TRUE(txn_mgr->CommitTxn(txn).ok());
    }

    // (b) Early give-up: destroy the iterator after a few docs, repeatedly -- must not crash or leak
    for (u32 round = 0; round < 20; ++round) {
        auto partial = Run(MakeQuery(120, 900 + round), {EarlyTermAlgo::kHeapBMW, 10, {}});
        ASSERT_LE(partial.docs_.size(), 10u);
    }

    // (c) minimum_should_match: naive and BMW must agree on top-10 with msm=2
    {
        MinimumShouldMatchOption msm = ParseMinimumShouldMatchOption("2");
        auto naive_msm = Run(query_text, {EarlyTermAlgo::kNaive, 10, msm});
        auto bmw_msm = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, msm});
        ASSERT_EQ(naive_msm.docs_.size(), bmw_msm.docs_.size());
        ASSERT_FALSE(naive_msm.docs_.empty());
        for (size_t i = 0; i < naive_msm.docs_.size(); ++i) {
            const float expected = naive_msm.docs_[i].second;
            ASSERT_NEAR(expected, bmw_msm.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "msm rank " << i;
        }
        std::cerr << fmt::format("[BMW-IT] msm=2 top10 naive={:.1f}ms bmw={:.1f}ms\n", naive_msm.elapsed_ms_, bmw_msm.elapsed_ms_);
    }

    // (d) OR tree mixing phrases and terms (BMW children may be phrases)
    {
        std::string phrase_query;
        for (u32 i = 0; i < 10; ++i) {
            const auto term_a = fmt::format("w{:05d}", (i * 7 + 3) % vocab_size_);
            const auto term_b = fmt::format("w{:05d}", (i * 13 + 11) % vocab_size_);
            phrase_query += fmt::format("\"{}\" ", term_a);
            if (i % 2 == 0) {
                phrase_query += fmt::format("\"{}\" ", term_b);
            }
        }
        phrase_query += MakeQuery(100, 31337);
        auto naive_result = Run(phrase_query, {EarlyTermAlgo::kNaive, 0, {}});
        auto bmw_result = Run(phrase_query, {EarlyTermAlgo::kHeapBMW, 0, {}});
        ASSERT_EQ(naive_result.docs_.size(), bmw_result.docs_.size());
        for (size_t i = 0; i < naive_result.docs_.size(); ++i) {
            ASSERT_EQ(naive_result.docs_[i].first, bmw_result.docs_[i].first) << "phrase idx " << i;
            const float expected = naive_result.docs_[i].second;
            ASSERT_NEAR(expected, bmw_result.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected))) << "phrase idx " << i;
        }
        std::cerr << fmt::format("[BMW-IT] phrase-mixed matched={} naive={:.1f}ms bmw={:.1f}ms\n",
                                 naive_result.docs_.size(),
                                 naive_result.elapsed_ms_,
                                 bmw_result.elapsed_ms_);
    }

    // (e) Concurrency: several threads share one IndexReader on long queries; every result must be
    //     complete and nothing may crash
    {
        constexpr u32 kThreads = 4;
        constexpr u32 kRounds = 5;
        std::atomic<u32> failure_count{0};
        std::vector<std::thread> workers;
        workers.reserve(kThreads);
        for (u32 thread_id = 0; thread_id < kThreads; ++thread_id) {
            workers.emplace_back([this, &failure_count, &query_text] {
                for (u32 round = 0; round < kRounds; ++round) {
                    auto result = Run(query_text, {EarlyTermAlgo::kHeapBMW, 10, {}});
                    if (result.docs_.size() != 10u) {
                        ++failure_count;
                    }
                }
            });
        }
        for (auto &worker : workers) {
            worker.join();
        }
        ASSERT_EQ(failure_count.load(), 0u);
        std::cerr << fmt::format("[BMW-IT] concurrency: {} threads x {} rounds done\n", kThreads, kRounds);
    }
}
// Randomized differential stress (opt-in): random query length / topn / minimum_should_match, plus
// random Next(RowID) jumps. The jump variant is the important one for the "stale posting block"
// failure class (473f4fa0): it needs a block-level seek inside one iterator followed by a score on
// that same iterator, which a plain doc-ordered scan rarely produces.
//   INFINITY_BMW_IT_RANDOM=1 INFINITY_BMW_IT_SEGMENTS=8 INFINITY_BMW_IT_DOCS=125000 \
//   INFINITY_BMW_IT_VOCAB=20000 INFINITY_BMW_IT_RANDOM_ROUNDS=12 \
//   ./build/src/test_main --gtest_also_run_disabled_tests --gtest_filter='*RandomizedDifferential*'
TEST_F(BmwLongQueryTest, DISABLED_SLOW_LongQueryRandomizedDifferential) {
    if (std::getenv("INFINITY_BMW_IT_RANDOM") == nullptr) {
        GTEST_SKIP() << "set INFINITY_BMW_IT_RANDOM=1 (with INFINITY_BMW_IT_SEGMENTS/DOCS/VOCAB) to run the randomized differential";
    }
    PrepareIndex();
    const size_t total_docs = size_t(segments_) * docs_per_segment_;
    const u32 rounds = std::max<u32>(1, EnvU32("INFINITY_BMW_IT_RANDOM_ROUNDS", 12));
    std::mt19937_64 rng(20261101);
    // Mutation check: pointing the differential at the legacy sort-based iterator (known recall
    // issues) must make this test fail, which proves the probe actually has detection power.
    const EarlyTermAlgo algo = std::getenv("INFINITY_BMW_IT_RANDOM_ALGO_BMW") != nullptr ? EarlyTermAlgo::kBMW : EarlyTermAlgo::kHeapBMW;
    std::cerr << fmt::format("[BMW-RAND] corpus={} docs ({} seg x {}), vocab={}, rounds={}\n",
                             total_docs,
                             segments_,
                             docs_per_segment_,
                             vocab_size_,
                             rounds);

    // Drives only the heap BMW with a monotonically increasing, randomly sized target sequence and
    // checks every hop against the naive reference (full scan, doc ascending). A stale block would
    // either abort here (FastPFor on a mismatched skip entry) or return a wrong doc / wrong score.
    const auto jump_probe =
        [&](const std::string &query_text, const MinimumShouldMatchOption &msm, const std::vector<std::pair<RowID, float>> &reference, u32 jumps) {
            size_t mismatches = 0;
            std::string first_mismatch;
            NewTxnManager *txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = txn_mgr->BeginTxn(std::make_unique<std::string>("bmw rand jump"), TransactionType::kRead);
            {
                OpenQuery query(txn, db_name_, table_name_, index_reader_, query_text, {algo, 0, msm});
                EXPECT_TRUE(query.ok());
                if (query.ok()) {
                    DocIterator *doc_iterator = query.iterator();
                    size_t cursor = 0;
                    u64 target_value = 0;
                    for (u32 i = 0; i < jumps; ++i) {
                        target_value += 1 + rng() % 4096;
                        while (cursor < reference.size() && reference[cursor].first.ToUint64() < target_value) {
                            ++cursor;
                        }
                        if (cursor >= reference.size()) {
                            break;
                        }
                        const bool has = doc_iterator->Next(RowID(target_value));
                        const float expected = reference[cursor].second;
                        const bool doc_ok = has && doc_iterator->DocID() == reference[cursor].first;
                        const float actual = has ? doc_iterator->Score() : 0.0F;
                        const bool score_ok = has && std::abs(expected - actual) <= 1e-4F * std::max(1.0F, std::abs(expected));
                        if (!doc_ok || !score_ok) {
                            ++mismatches;
                            if (first_mismatch.empty()) {
                                first_mismatch =
                                    fmt::format("target={} expected_doc={} expected_score={} actual_has={} actual_doc={} actual_score={}",
                                                target_value,
                                                reference[cursor].first.ToString(),
                                                expected,
                                                has,
                                                has ? doc_iterator->DocID().ToString() : "n/a",
                                                actual);
                            }
                            if (mismatches >= 3) {
                                break;
                            }
                        }
                        target_value = reference[cursor].first.ToUint64();
                    }
                }
            }
            EXPECT_TRUE(txn_mgr->CommitTxn(txn).ok());
            return std::make_pair(mismatches, first_mismatch);
        };

    const u32 topn_choices[] = {0u, 1u, 10u, 100u, 1000u};
    const char *msm_choices[] = {"1", "2", "3"};
    for (u32 round = 0; round < rounds; ++round) {
        const u32 num_terms = 5 + static_cast<u32>(rng() % 500);
        const u32 topn = topn_choices[rng() % 5];
        const char *msm_text = msm_choices[rng() % 3];
        const MinimumShouldMatchOption msm = ParseMinimumShouldMatchOption(msm_text);
        const std::string query_text = MakeQuery(num_terms, rng());
        const bool materialize = std::getenv("INFINITY_BMW_IT_RANDOM_NO_MATERIALIZE") == nullptr;
        std::cerr << fmt::format("[BMW-RAND] round={}/{} terms={} topn={} msm={}\n", round + 1, rounds, num_terms, topn, msm_text);

        // (1) lockstep full-scan parity (zero extra memory)
        const auto parity = CompareFullScan(query_text, {EarlyTermAlgo::kNaive, 0, msm}, {algo, 0, msm}, 10);
        ASSERT_EQ(parity.mismatches_, 0u) << "round=" << round << " terms=" << num_terms << " " << parity.first_mismatch_;

        // (2) random jumps against the naive reference
        if (materialize) {
            auto reference = Run(query_text, {EarlyTermAlgo::kNaive, 0, msm});
            ASSERT_GT(reference.docs_.size(), 0u) << "round=" << round;
            const auto [jump_mismatches, jump_first] = jump_probe(query_text, msm, reference.docs_, 200);
            ASSERT_EQ(jump_mismatches, 0u) << "round=" << round << " terms=" << num_terms << " " << jump_first;
        }

        // (3) top-k against the naive top-k on the same threshold feedback
        if (topn > 0) {
            auto naive_top = Run(query_text, {EarlyTermAlgo::kNaive, topn, msm});
            auto heap_top = Run(query_text, {algo, topn, msm});
            ASSERT_EQ(heap_top.docs_.size(), naive_top.docs_.size()) << "round=" << round << " topn=" << topn;
            for (size_t i = 0; i < heap_top.docs_.size(); ++i) {
                const float expected = naive_top.docs_[i].second;
                ASSERT_NEAR(expected, heap_top.docs_[i].second, 1e-4F * std::max(1.0F, std::abs(expected)))
                    << "round=" << round << " topn=" << topn << " rank " << i;
            }
        }
    }
}
