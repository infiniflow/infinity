//
// Created by jinhai on 23-2-26.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"
#include "common/types/internal_types.h"
#include "parser/parser_result.h"
#include "parser/sql_parser.h"


class SQLParserTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

TEST_F(SQLParserTest, test1) {
    using namespace infinity;

    Vector<String> inputs;
    inputs.emplace_back("create collection c1;");
    inputs.emplace_back("create schema s1;");
    inputs.emplace_back("create collection if not exists c2;");
    inputs.emplace_back("create schema if not exists s2;");

    inputs.emplace_back("drop collection c1;");
    inputs.emplace_back("drop schema s1;");
    inputs.emplace_back("drop collection if exists c2;");
    inputs.emplace_back("drop schema if exists s2;");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header, delimiter '|');");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header, delimiter '|');");

    inputs.emplace_back("copy t2 from '/usr/filename' with (format xml);");
    inputs.emplace_back("copy t2 to '/usr/filename' with (format xml);");
    inputs.emplace_back("show tables;");
    inputs.emplace_back("describe t1;");
    inputs.emplace_back("describe s1.t1;");

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    for(const String& input: inputs) {
        parser->Parse(input, result);
        std::cout << result->ToString() << std::endl;
        result->Reset();
    }

}