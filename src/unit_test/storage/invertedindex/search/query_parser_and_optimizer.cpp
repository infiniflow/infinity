//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "unit_test/base_test.h"

import stl;
import search_driver;
import query_node;
import infinity_exception;

using namespace infinity;

class QueryParserAndOptimizerTest : public BaseTest {};

int ParseAndOptimizeFromStream(const SearchDriver &driver, std::istream &ist) {
    // read and parse line by line, ignoring empty lines and comments
    std::string line;
    while (std::getline(ist, line)) {
        size_t firstNonBlank = line.find_first_not_of(" \t");
        if (firstNonBlank == std::string::npos || line[firstNonBlank] == '#') {
            continue;
        }
        line = line.substr(firstNonBlank);
        std::cerr << "---query: ###" << line << "###" << std::endl;
        std::unique_ptr<QueryNode> parser_result = driver.ParseSingle(line);
        if (!parser_result) {
            std::cerr << "---parser failed" << std::endl;
            return -1;
        } else {
            std::cerr << "---parser accepted" << std::endl;
            std::cerr << "---parser output tree:" << std::endl;
            parser_result->PrintTree(std::cerr);
            std::cerr << std::endl;
            std::unique_ptr<QueryNode> optimizer_result;
            bool recoverable_exception = false;
            try {
                optimizer_result = QueryNode::GetOptimizedQueryTree(std::move(parser_result));
            } catch (const RecoverableException &e) {
                recoverable_exception = true;
                std::cerr << "---optimizer failed with recoverable exception:\n---" << e.what() << '\n' << std::endl;
            }
            if (optimizer_result) {
                std::cerr << "---optimizer accepted" << std::endl;
                std::cerr << "---optimizer output tree:" << std::endl;
                optimizer_result->PrintTree(std::cerr);
                std::cerr << std::endl;
            } else if (!recoverable_exception) {
                std::cerr << "---optimizer failed" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}

TEST_F(QueryParserAndOptimizerTest, test1) {
    using namespace infinity;

    std::string row_quires = R"##(
#basic_filter with implicit field
dune

#basic_filter with explicit field
name:dune

#basic_filter_boost with implicit field
dune^1.2

#basic_filter_boost with explicit field
name:dune^1.2

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

#test invalid not query
NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR ee:ff^1.2
(NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2
(NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2

#test optimization rules
(dune^1.2 AND NOT name:god^2 AND NOT kddd:ss^4 OR ee:ff^1.2)^1.3
(dune^1.2 AND NOT (name:god^2 || kddd:ss^4) OR ee:ff^1.2)^1.3
(dune^1.2 AND (NOT name:god^2 || NOT kddd:ss^4) AND ee:ff^1.2)^1.3
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) OR NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 AND NOT kddd:ss^4) AND NOT ee:ff^1.2)
sda:rtw AND ((NOT name:god^2 OR NOT kddd:ss^4) OR NOT ee:ff^1.2)
    )##";

    Map<String, String> column2analyzer;
    String default_field("body");
    SearchDriver driver(column2analyzer, default_field);
    IStringStream iss(row_quires);
    int rc = ParseAndOptimizeFromStream(driver, iss);
    EXPECT_EQ(rc, 0);
}
