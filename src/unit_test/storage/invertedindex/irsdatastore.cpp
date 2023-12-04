#include "analysis/analyzers.hpp"
#include "analysis/segmentation_token_stream.hpp"
#include "index/norm.hpp"
#include "index/segment_writer.hpp"
#include "search/bm25.hpp"
#include "search/filter.hpp"
#include "search/score.hpp"
#include "store/fs_directory.hpp"
#include "unit_test/base_test.h"
#include "utils/text_format.hpp"
#include "utils/type_info.hpp"
#include <fstream>
#include <iostream>
#include <string>

import stl;
import iresearch_document;
import iresearch_analyzer;
import third_party;

using namespace infinity;

class IRSDataStore {
public:
    IRSDataStore(const String &table_name, const String &directory);
    struct DataSnapshot {
        DataSnapshot(IRSDirectoryReader &&reader) : reader_(Move(reader)) {}
        DataSnapshot &operator=(DataSnapshot &&rhs) noexcept {
            if (this != &rhs) {
                reader_ = Move(rhs.reader_);
            }
            return *this;
        }

        IRSDirectoryReader reader_;
    };
    using DataSnapshotPtr = SharedPtr<IRSDataStore::DataSnapshot>;

    IRSIndexWriter::ptr GetWriter() { return index_writer_; }

    auto const &GetDirectoryReader() const { return snapshot_->reader_; }

    void StoreSnapshot(DataSnapshotPtr snapshot);

    IRSDataStore::DataSnapshotPtr LoadSnapshot() const;

    void Commit();

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
    auto open_mode = irs::OM_CREATE;

    IRSIndexReaderOptions reader_options;
    auto format = irs::formats::get("1_5simd");
    irs_directory_ = MakeUnique<irs::FSDirectory>(directory_.c_str());

    IRSIndexWriterOptions options;
    options.segment_pool_size = 1; // number of index threads
    options.reader_options = reader_options;
    options.segment_memory_max = 2 * (1 << 20);   // 128K
    options.lock_repository = false;              //?
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

    index_writer_ = IRSIndexWriter::Make(*(irs_directory_), Move(format), OpenMode(open_mode), options);
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));

    AnalyzerPool::instance().Set(SEGMENT);
    StoreSnapshot(data);
}

void IRSDataStore::StoreSnapshot(DataSnapshotPtr snapshot) { snapshot_ = snapshot; }

void IRSDataStore::Commit() {
    UniqueLock<Mutex> lk(commit_mutex_);
    index_writer_->Commit();
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));
    StoreSnapshot(data);
}

class IRSDatastoreTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(IRSDatastoreTest, test1) {
    constexpr static Array<IRSTypeInfo::type_id, 1> TEXT_FEATURES{IRSType<Norm>::id()};
    constexpr static Array<IRSTypeInfo::type_id, 1> NUMERIC_FEATURES{IRSType<GranularityPrefix>::id()};

    static Features text_features{TEXT_FEATURES.data(), TEXT_FEATURES.size()};
    static Features numeric_features{NUMERIC_FEATURES.data(), NUMERIC_FEATURES.size()};

    IRSDataStore datastore("wiki", "./");

    UniquePtr<IRSAnalyzer> stream = AnalyzerPool::instance().Get(SEGMENT);
    auto ctx = datastore.index_writer_->GetBatch();

    std::fstream fin;
    std::istream *in;
    fin.open("/home/infominer/codebase/workspace/infinity-debug/test/data/csv/enwiki_9999.csv", std::fstream::in);
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
            auto field = MakeShared<TextField>("title", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(*field);
        }
        {
            auto field = MakeShared<TextField>("date", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(*field);
        }
        {
            auto field = MakeShared<TextField>("body", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            std::getline(line_stream, field->f_, '\t');
            doc.Insert<irs::Action::INDEX>(*field);
        }
    }
    /*

    for (int i = 1; i < 10000; ++i) {
        auto doc = ctx.Insert(i);
        {
            auto field = MakeShared<TextField>("body", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            field->f_ = "hello, world, a, b, c ";
            doc.Insert<irs::Action::INDEX>(*field);
        }

        {
            auto field = MakeShared<TextField>("title", irs::IndexFeatures::FREQ | irs::IndexFeatures::POS, text_features, stream.get());
            field->f_ = "Anarchism";
            doc.Insert<irs::Action::INDEX>(*field);
        }
    }*/
    datastore.Commit();
}
