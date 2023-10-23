//
// Created by jinhai on 23-3-5.
//

#include "base_test.h"
#include "parser/sql_parser.h"

#include "main/infinity.h"
#include "bin/compilation_config.h"

class SQLFileParsingTest : public BaseTest {};

TEST_F(SQLFileParsingTest, tpch) {
    using namespace infinity;

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String path = String(TEST_DATA_PATH) + "/tpch";
    std::filesystem::path file_path(path);

    for (const auto &p : std::filesystem::recursive_directory_iterator(file_path)) {
        //        std::cout << p.path().c_str() << std::endl;
        String filename = p.path().filename();
        if (filename == "README.md") {
            continue;
        }
        std::ifstream t(p.path().c_str());
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

TEST_F(SQLFileParsingTest, hyrise) {
    using namespace infinity;

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String good_sql = String(TEST_DATA_PATH) + "/hyrise/good.sql";
    String bad_sql = String(TEST_DATA_PATH) + "/hyrise/bad.sql";
    std::filesystem::path good_sql_path(good_sql);
    std::filesystem::path bad_sql_path(bad_sql);

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

TEST_F(SQLFileParsingTest, infinity) {
    using namespace infinity;

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    // Get all tpch sql text;
    String good_sql = String(TEST_DATA_PATH) + "/infinity/good.sql";
    std::filesystem::path good_sql_path(good_sql);

    Vector<String> sqls;
    ReadSQLs(good_sql_path, sqls);
    for (auto &input_sql : sqls) {
        //        std::cout << input_sql << std::endl;
        parser->Parse(input_sql, result);
        result->Reset();
    }
}
