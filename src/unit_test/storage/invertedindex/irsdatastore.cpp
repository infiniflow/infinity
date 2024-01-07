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

#include "analysis/analyzers.hpp"
#include "analysis/segmentation_token_stream.hpp"
#include "index/norm.hpp"
#include "index/segment_writer.hpp"
#include "parser/search_options.h"
#include "search/bm25.hpp"
#include "search/filter.hpp"
#include "search/proxy_filter.hpp"
#include "search/range_filter.hpp"
#include "search/score.hpp"
#include "search/term_filter.hpp"
#include "search/terms_filter.hpp"
#include "store/fs_directory.hpp"
#include "unit_test/base_test.h"
#include "utils/async_utils.hpp"
#include "utils/text_format.hpp"
#include "utils/type_info.hpp"

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>

import stl;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import logger;

using namespace infinity;

using IrsFilter = irs::filter;

using ScoredId = Pair<float, u32>;
using ScoredIds = Vector<ScoredId>;
static const String DEFAULT_SCORER("bm25");
static const String DEFAULT_SCORER_ARG("");
static const SizeT DEFAULT_TOPN(100);
// constexpr SizeT DEFAULT_COMMIT_INTERVAL = 10000;

class IRSDataStore {
public:
    IRSDataStore(const String &table_name, const String &directory);
    struct DataSnapshot {
        DataSnapshot(IRSDirectoryReader &&reader) : reader_(std::move(reader)) {}
        DataSnapshot &operator=(DataSnapshot &&rhs) noexcept {
            if (this != &rhs) {
                reader_ = std::move(rhs.reader_);
            }
            return *this;
        }

        IRSDirectoryReader reader_;
    };
    using DataSnapshotPtr = SharedPtr<IRSDataStore::DataSnapshot>;

    void Open(bool reopen);

    IRSIndexWriter::ptr GetWriter() { return index_writer_; }

    auto const &GetDirectoryReader() const { return snapshot_->reader_; }

    void StoreSnapshot(DataSnapshotPtr snapshot);

    IRSDataStore::DataSnapshotPtr LoadSnapshot() const;

    void Commit();

    int Search(IrsFilter *flt, const Map<String, String> &options, ScoredIds &sorted);

public:
    String directory_;
    Path path_;
    IRSDirectory::ptr irs_directory_;
    IRSIndexWriter::ptr index_writer_;
    Mutex commit_mutex_;
    DataSnapshotPtr snapshot_;
};

IRSDataStore::IRSDataStore(const String &table_name, const String &directory) {
    path_ = Path(directory) / table_name;
    directory_ = path_.string();
    std::error_code ec;
    std::filesystem::remove_all(path_);
    std::filesystem::create_directories(path_, ec);
    irs_directory_ = MakeUnique<irs::FSDirectory>(directory_.c_str());
    AnalyzerPool::instance().Set(SEGMENT);
    Open(false);
}

void IRSDataStore::Open(bool reopen) {
    auto open_mode = reopen ? (irs::OM_CREATE | irs::OM_APPEND) : irs::OM_CREATE;

    IRSIndexReaderOptions reader_options;
    auto format = irs::formats::get("1_5simd");

    IRSIndexWriterOptions options;
    options.segment_pool_size = 1; // number of index threads
    options.reader_options = reader_options;
    options.segment_memory_max = 2 * (1 << 20); // 128K
    options.lock_repository = false;            //?
    options.comparator = nullptr;
    options.features = [](irs::type_info::type_id id) {
        const irs::ColumnInfo info{irs::type<irs::compression::none>::get(), {}, false};

        if (irs::type<irs::Norm>::id() == id) {
            return std::make_pair(info, &irs::Norm::MakeWriter);
        }

        if (irs::type<irs::Norm2>::id() == id) {
            return std::make_pair(info, &irs::Norm2::MakeWriter);
        }

        return std::make_pair(info, irs::FeatureWriterFactory{});
    };

    index_writer_ = IRSIndexWriter::Make(*(irs_directory_), std::move(format), OpenMode(open_mode), options);
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(std::move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::StoreSnapshot(DataSnapshotPtr snapshot) { std::atomic_store_explicit(&snapshot_, std::move(snapshot), MemoryOrderRelease); }

void IRSDataStore::Commit() {
    UniqueLock<Mutex> lk(commit_mutex_);
    index_writer_->Commit();
    auto reader = index_writer_->GetSnapshot();
    reader->Reopen();
    std::cout << "Index stats:"
              << "\nsegments=" << reader->size() << "\ndocs=" << reader->docs_count() << "\nlive-docs=" << reader->live_docs_count() << std::endl;
    std::cout << "reader" << std::endl;

    for (auto &segment : reader) {
        std::cout << segment.Meta().name << std::endl;
    }
    auto data = MakeShared<DataSnapshot>(std::move(reader));
    StoreSnapshot(data);
}

enum class ExecutionMode { kAll, kStrictTop, kTop };

int IRSDataStore::Search(IrsFilter *flt, const Map<String, String> &options, ScoredIds &sorted) {
    irs::WandContext wand{.index = 0, .strict = false};

    String scorer(DEFAULT_SCORER);
    String scorer_arg(DEFAULT_SCORER_ARG);
    if (auto it = options.find("scorer"); it != options.end()) {
        scorer = it->second;
    }
    if (auto it = options.find("scorer_arg"); it != options.end()) {
        scorer_arg = it->second;
    }
    auto arg_format = irs::type<irs::text_format::csv>::get();
    auto scr = irs::scorers::get(scorer, arg_format, scorer_arg);
    if (!scr) {
        return -1;
    }

    const IRSDirectoryReader &reader = GetDirectoryReader();
    irs::Scorers order = irs::Scorers::Prepare(scr.get());

    irs::filter::prepared::ptr filter = flt->prepare({
        .index = reader,
        .scorers = order,
    });

    SizeT topn(DEFAULT_TOPN);
    if (auto it = options.find("topn"); it != options.end()) {
        topn = StrToInt(it->second);
    }

    sorted.reserve(topn);
    for (auto left = topn; auto &segment : reader) {
        auto docs = filter->execute(irs::ExecutionContext{.segment = segment, .scorers = order, .wand = wand});
        IRS_ASSERT(docs);

        const irs::document *doc = irs::get<irs::document>(*docs);
        const irs::score *score = irs::get<irs::score>(*docs);
        auto *threshold = irs::get_mutable<irs::score>(docs.get());
        if (!left && threshold) {
            IRS_ASSERT(!sorted.empty());
            IRS_ASSERT(std::is_heap(std::begin(sorted),
                                    std::end(sorted),
                                    [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                        return lhs.first > rhs.first;
                                    }));
            threshold->Min(sorted.front().first);
        }

        for (float_t score_value; docs->next();) {
            (*score)(&score_value);
            if (left) {
                sorted.emplace_back(score_value, doc->value);
                if (0 == --left) {
                    std::make_heap(std::begin(sorted),
                                   std::end(sorted),
                                   [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                       return lhs.first > rhs.first;
                                   });
                    threshold->Min(sorted.front().first);
                }
            } else if (sorted.front().first < score_value) {
                std::pop_heap(std::begin(sorted),
                              std::end(sorted),
                              [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                  return lhs.first > rhs.first;
                              });
                auto &[score, doc_id] = sorted.back();
                score = score_value;
                doc_id = doc->value;
                std::push_heap(std::begin(sorted),
                               std::end(sorted),
                               [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept {
                                   return lhs.first > rhs.first;
                               });
                threshold->Min(sorted.front().first);
            }
        }
    }
    std::sort(
        std::begin(sorted),
        std::end(sorted),
        [](const std::pair<float_t, irs::doc_id_t> &lhs, const std::pair<float_t, irs::doc_id_t> &rhs) noexcept { return lhs.first > rhs.first; });
    return 0;
}

class IRSDatastoreTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

static irs::bstring toBstring(const std::string &str) {
    irs::bstring converted_str((uint8_t *)str.c_str(), str.length());
    return converted_str;
}

TEST_F(IRSDatastoreTest, test1) {
    constexpr static Array<IRSTypeInfo::type_id, 1> TEXT_FEATURES{IRSType<Norm>::id()};
    static Features text_features{TEXT_FEATURES.data(), TEXT_FEATURES.size()};
    IRSDataStore datastore("wiki", "./");
    UniquePtr<IRSAnalyzer> stream = AnalyzerPool::instance().Get(SEGMENT);
    {
        auto ctx = datastore.index_writer_->GetBatch();
#if 0
    std::fstream fin;
    std::istream *in;
    fin.open("../test/data/csv/enwiki_9999.csv", std::fstream::in);
    if (!fin) {
        return;
    }
    in = &fin;
    for (auto i = 1; i < 10000; ++i) {
        std::string line;
        if (std::getline(*in, line).eof()) {
            break;
        }
        std::stringstream line_stream(line);
        auto doc = ctx.Insert(i);
        {
            auto field = MakeUnique<TextField>("title", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(field.get());
        }
        {
            auto field = MakeUnique<TextField>("date", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(field.get());
        }
        {
            auto field = MakeUnique<TextField>("body", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(field.get());
        }
    }

#else

        for (int i = 1; i < 10000; ++i) {
            auto doc = ctx.Insert(i);
            {
                auto field = MakeUnique<TextField>("body", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
                field->f_ = "hello, world, a, b, c ";
                doc.Insert<irs::Action::INDEX>(field.get());
            }

            {
                auto field = MakeUnique<TextField>("title", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
                field->f_ = "Anarchism";
                doc.Insert<irs::Action::INDEX>(field.get());
            }
        }
#endif
    }
    datastore.Commit();

    UniquePtr<irs::filter> flt;
    auto query = std::make_unique<irs::by_term>();
    query->mutable_options()->term = toBstring("hello");
    *query->mutable_field() = "body";
    flt = std::move(query);

    SearchOptions search_ops("");
    ScoredIds result;
    auto rc = datastore.Search(flt.get(), search_ops.options_, result);
    ASSERT_TRUE(rc == 0);
    ASSERT_TRUE(result.size() == 100);
}
