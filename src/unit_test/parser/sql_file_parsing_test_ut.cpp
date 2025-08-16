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

#ifdef CI
#include "unit_test/gtest_expand.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.sql_file_parsing_test;

import :ut.base_test;
import :infinity_context;
import :infinity_exception;
import :logger;
import :null_value;
#endif

import third_party;
import compilation_config;
import global_resource_usage;
import sql_parser;
import parser_result;

using namespace infinity;
class SQLFileParsingTest : public BaseTest {};

TEST_F(SQLFileParsingTest, tpch) {
    using namespace infinity;

    std::shared_ptr<SQLParser> parser = std::make_shared<SQLParser>();
    std::shared_ptr<ParserResult> result = std::make_shared<ParserResult>();

    auto GetFilesFromDir = [](const std::string &path) {
        std::vector<std::string> result;
        for (auto &i : std::filesystem::directory_iterator(path)) {
            result.emplace_back(i.path().string());
        }
        return result;
    };

    // Get all tpch sql text;
    std::string path = std::string(test_data_path()) + "/tpch";
    std::vector<std::string> files = GetFilesFromDir(path);
    size_t file_count = files.size();
    for (size_t idx = 0; idx < file_count; ++idx) {
        const std::string &filename = files[idx];
        if (filename == "README.md") {
            continue;
        }
        std::ifstream t(filename);
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        parser->Parse(input_sql, result.get());
        result->Reset();
    }
}

namespace infinity {

void ReadSQLs(const std::string &file_path, std::vector<std::string> &sqls) {
    std::ifstream infile(file_path);
    std::string line;
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

TEST_F(SQLFileParsingTest, hyrise) {
    using namespace infinity;

    std::shared_ptr<SQLParser> parser = std::make_shared<SQLParser>();
    std::shared_ptr<ParserResult> result = std::make_shared<ParserResult>();

    // Get all tpch sql text;
    std::string good_sql = std::string(test_data_path()) + "/hyrise/good.sql";
    std::string bad_sql = std::string(test_data_path()) + "/hyrise/bad.sql";
    std::filesystem::path good_sql_path(good_sql);
    std::filesystem::path bad_sql_path(bad_sql);

    std::vector<std::string> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result.get());
        result->Reset();
    }

    ReadSQLs(bad_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result.get());
        result->Reset();
    }
}

TEST_F(SQLFileParsingTest, infinity) {
    using namespace infinity;

    std::shared_ptr<SQLParser> parser = std::make_shared<SQLParser>();
    std::shared_ptr<ParserResult> result = std::make_shared<ParserResult>();

    // Get all tpch sql text;
    std::string good_sql = std::string(test_data_path()) + "/infinity/good.sql";
    std::filesystem::path good_sql_path(good_sql);

    std::vector<std::string> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result.get());
        result->Reset();
    }
}
