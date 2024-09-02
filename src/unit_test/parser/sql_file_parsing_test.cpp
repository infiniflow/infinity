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

#include "gtest/gtest.h"
import base_test;

import compilation_config;
import infinity_context;
import infinity_exception;

import global_resource_usage;
import third_party;
import sql_parser;
import logger;
import null_value;
import stl;
import parser_result;

using namespace infinity;
class SQLFileParsingTest : public BaseTest {};

TEST_F(SQLFileParsingTest, tpch) {
    using namespace infinity;

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String path = String(test_data_path()) + "/tpch";
    Vector<String> files = GetFilesFromDir(path);
    SizeT file_count = files.size();
    for (SizeT idx = 0; idx < file_count; ++idx) {
        const String &filename = files[idx];
        if (filename == "README.md") {
            continue;
        }
        std::ifstream t(filename);
        String input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        parser->Parse(input_sql, result.get());
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

TEST_F(SQLFileParsingTest, hyrise) {
    using namespace infinity;

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

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String good_sql = String(test_data_path()) + "/infinity/good.sql";
    Path good_sql_path(good_sql);

    Vector<String> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result.get());
        result->Reset();
    }
}
