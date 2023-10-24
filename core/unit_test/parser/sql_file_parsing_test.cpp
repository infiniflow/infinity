//
// Created by jinhai on 23-3-5.
//

#include "unit_test/base_test.h"

import compilation_config;
import infinity;
import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import null_value;
import stl;

class SQLFileParsingTest1 : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(SQLFileParsingTest1, tpch) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String path = String(test_data_path()) + "/tpch";
    Vector<String> files = GetFilesFromDir(path);
    SizeT file_count = files.size();
    for(SizeT idx = 0; idx < file_count; ++ idx) {
        const String& filename = files[idx];
        if (filename == "README.md") {
            continue;
        }
        std::ifstream t(filename);
        String input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        parser->Parse(input_sql, result);
        result->Reset();
    }
}

namespace infinity {

void ReadSQLs(const String &file_path, Vector<String> &sqls) {
    std::ifstream infile(file_path);
    String line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        // Skip comments.
        if (line[0] == '#' || (line[0] == '-' && line[1] == '-')) {
            continue;
        }
        if (line[0] == '!') {
            line = line.substr(1);
        }
        sqls.emplace_back(line);
    }
}

} // namespace infinity

TEST_F(SQLFileParsingTest1, hyrise) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String good_sql = String(test_data_path()) + "/hyrise/good.sql";
    String bad_sql = String(test_data_path()) + "/hyrise/bad.sql";
    Path good_sql_path(good_sql);
    Path bad_sql_path(bad_sql);

    Vector<String> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result);
        result->Reset();
    }

    ReadSQLs(bad_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result);
        result->Reset();
    }
}

TEST_F(SQLFileParsingTest1, infinity) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String good_sql = String(test_data_path()) + "/infinity/good.sql";
    Path good_sql_path(good_sql);

    Vector<String> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result);
        result->Reset();
    }
}
