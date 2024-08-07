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

#include "unit_test/base_test.h"

import stl;
import search_driver;
import query_node;
import term;
import analyzer;
import infinity_exception;
import global_resource_usage;
import infinity_context;
import logger;
import third_party;

using namespace infinity;

class SearchDriverTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        RemoveDbDirs();
        BaseTest::TearDown();
    }
};

struct LogHelper {
    void Reset() {
        LOG_INFO(std::move(oss).str());
        oss.str("");
        oss.clear();
    }
    ~LogHelper() { LOG_INFO(std::move(oss).str()); }
    OStringStream oss;
};

int ParseStream(const SearchDriver &driver, std::istream &ist) {
    LogHelper log_helper;
    auto &oss = log_helper.oss;
    // read and parse line by line, ignoring empty lines and comments
    std::string line;
    while (std::getline(ist, line)) {
        size_t firstNonBlank = line.find_first_not_of(" \t");
        if (firstNonBlank == std::string::npos || line[firstNonBlank] == '#') {
            continue;
        }
        line = line.substr(firstNonBlank);
        oss << "---query: ###" << line << "###" << std::endl;
        std::unique_ptr<QueryNode> parser_result = driver.ParseSingle(line);
        if (!parser_result) {
            oss << "---failed" << std::endl;
            return -1;
        } else {
            oss << "---accepted" << std::endl;
            oss << "---parser output tree:" << std::endl;
            parser_result->PrintTree(oss);
            oss << std::endl;
        }
    }
    return 0;
}

TEST_F(SearchDriverTest, good_test1) {
    using namespace infinity;

    std::string row_quires = R"##(
#basic_filter with implicit field
dune
123.456

#basic_filter with explicit field
name:dune
num:123.456

#basic_filter_boost with implicit field
dune^1.2
123.456^7.8

#basic_filter_boost with explicit field
name:dune^1.2
num:123.456^7.8

#term
NOT name:microsoft^1.2
-name:google^1.2
!name:microsoft^1.2
(name:star OR name:duna)
(dune god)
(dune god)^1.3
!(dune god)^1.3

#clause
foo AND name:duna^1.2
foo +bar
duna^2 AND foo
duna AND foo
(dune god) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
dune god
dune OR god
name:star OR name:duna
_exists_:"author" AND page_count:yyy AND (name:star OR name:duna)
_exists_:"author" AND page_count:zzz^1.3 AND (name:star^0.1 OR name:duna^1.2)^1.2

#query
(dune^1.2 AND NOT name:god^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(dune^1.2 AND NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(dune^1.2 AND (NOT name:god^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#escape reserved characters per https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
first\ name:Alice
book\*:quick
DS\-K3AJ303/Dm140
DS\-K3AJ303\/Dm140
\(DS\-K3AJ303\/Dm140\)

#quote
"first name":Alice
"book*":quick
"DS-K3AJ303/Dm140"

#sloppy phrase
"a b"
"a b"~0
"a b"~1
"a b"~2
"a b c"~4
    )##";

    Map<String, String> column2analyzer;
    String default_field("body");
    SearchDriver driver(column2analyzer, default_field);
    IStringStream iss(row_quires);
    int rc = ParseStream(driver, iss);
    EXPECT_EQ(rc, 0);
}

TEST_F(SearchDriverTest, good_test2) {
    using namespace infinity;

    std::string row_quires = R"##(
#basic_filter with implicit field
芯片

#basic_filter with explicit field
name:芯片

#basic_filter_boost with implicit field
芯片^1.2

#basic_filter_boost with explicit field
name:芯片^1.2

#term
NOT name:microsoft^1.2
-name:芯片^1.2
!name:微软^1.2
(name:星空 OR name:邓肯)
(邓肯 上帝)
(邓肯 上帝)^1.3
!(邓肯 上帝)^1.3

#clause
foo AND name:邓肯^1.2
foo +bar
邓肯^2 AND foo
邓肯 AND foo
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
邓肯 上帝
邓肯 OR 上帝
name:星空 OR name:邓肯
_exists_:"author" AND page_count:yyy AND (name:星空 OR name:邓肯)
_exists_:"author" AND page_count:zzz^1.3 AND (name:星空^0.1 OR name:邓肯^1.2)^1.2

#query
(邓肯^1.2 AND NOT name:上帝^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(邓肯^1.2 AND NOT (name:上帝^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(邓肯^1.2 AND (NOT name:上帝^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#quote
吉祥物“羽宝”头部
nanjing吉祥物"羽宝"头部head "DS-K3AJ303/Dm140"
吉祥物nanjing"DS-K3AJ303/Dm140"头部
graphic cards
    )##";

    Map<String, String> column2analyzer;
    column2analyzer["body"] = "chinese";
    String default_field("body");
    SearchDriver driver(column2analyzer, default_field);
    IStringStream iss(row_quires);
    try {
        int rc = ParseStream(driver, iss);
        EXPECT_EQ(rc, 0);
    } catch (RecoverableException &e) {
        std::cerr << fmt::format("RecoverableException: {} {}\n", e.what(), long(e.ErrorCode()));
    }
}

TEST_F(SearchDriverTest, operator_option_test) {
    using namespace infinity;
    std::string row_quires = R"##(
#basic_filter_boost with explicit field
name:芯片^1.2
name:dune^1.2
num:123.456^7.8

#clause
(邓肯 上帝) AND (foo bar)
_exists_:"author" AND page_count:xxx AND name:star^1.3

#query
(邓肯^1.2 AND (NOT name:上帝^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3

#quote
TO BE OR NOT TO BE
吉祥物“羽宝”头部
nanjing吉祥物"羽宝"头部head "DS-K3AJ303/Dm140"
吉祥物nanjing"DS-K3AJ303/Dm140"头部
graphic cards
    )##";

    static constexpr FulltextQueryOperatorOption ops[] = {FulltextQueryOperatorOption::kOr, FulltextQueryOperatorOption::kAnd};
    static constexpr const char *ops_chars[] = {"OR", "AND"};
    Map<String, String> column2analyzer;
    column2analyzer["body"] = "chinese";
    String default_field("body");
    for (size_t i = 0; i < std::size(ops); ++i) {
        const auto op = ops[i];
        LOG_INFO(fmt::format("Test With Operator Option: {}", ops_chars[i]));
        SearchDriver driver(column2analyzer, default_field, op);
        IStringStream iss(row_quires);
        try {
            int rc = ParseStream(driver, iss);
            EXPECT_EQ(rc, 0);
        } catch (RecoverableException &e) {
            std::cerr << fmt::format("RecoverableException: {} {}\n", e.what(), long(e.ErrorCode()));
        }
    }
}