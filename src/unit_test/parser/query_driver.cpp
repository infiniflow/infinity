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

import third_party;
import parser;
import stl;
import infinity_context;
import query_driver;

class QueryDriverTest : public BaseTest {};

TEST_F(QueryDriverTest, good_test1) {
    using namespace infinity;

    std::string row_quires = R"##(
#basic_filter with implicit field
dune
/star.*/
duna~
duna~2
=123
=google
<123
<google
>123
>google
<=123.456
<= google
>= 123.456
>= google
{TO 123.456}
{TO google}
[-1000 TO]
[google TO]
{-1000 TO 123.456]
{microsoft TO google}
[-1000 TO 123.456}
{microsoft TO google}

#basic_filter with explicit field
name:dune
name:/star.*/
name:duna~
name:=123
name:=google
name:<123
name:<google
name:>123
name:>google
name:<= 123.456
name:<= google
name:>= 123.456
name:>= google
name:{TO 123.456}
name:{TO google}
name:[-1000 TO]
name:[google TO]
name:{-1000 TO 123.456]
name:{microsoft TO google}
name:[-1000 TO 123.456}
name:{microsoft TO google}

#basic_filter_boost with implicit field
dune^1.2
/star.*/^1.2
duna~^1.2
=123^1.2
=google^1.2
<123^1.2
<google^1.2
>123^1.2
>google^1.2
<=123.456^1.2
<= google^1.2
>= 123.456^1.2
>= google^1.2
{TO 123.456}^1.2
{TO google}^1.2
[-1000 TO]^1.2
[google TO]^1.2
{-1000 TO 123.456]^1.2
{microsoft TO google}^1.2
[-1000 TO 123.456}^1.2
{microsoft TO google}^1.2

#basic_filter_boost with explicit field
name:dune^1.2
name:/star.*/^1.2
name:duna~^1.2
name:=123^1.2
name:=google^1.2
name:<123^1.2
name:<google^1.2
name:>123^1.2
name:>google^1.2
name:<= 123.456^1.2
name:<= google^1.2
name:>= 123.456^1.2
name:>= google^1.2
name:{TO 123.456}^1.2
name:{TO google}^1.2
name:[-1000 TO]^1.2
name:[google TO]^1.2
name:{-1000 TO 123.456]^1.2
name:{microsoft TO google}^1.2
name:[-1000 TO 123.456}^1.2
name:{microsoft TO google}^1.2

#term
NOT name:{microsoft TO google}^1.2
-name:{microsoft TO google}^1.2
!name:{microsoft TO google}^1.2
(duna~2)
(name:duna~ ) 
(name:/star.*/ OR name:duna~)
(dune god)
(dune god)^1.3
!(dune god)^1.3

#clause
foo AND name:duna~^1.2
foo +bar
duna~2 AND foo
duna~ AND foo
(dune god) AND (foo bar)
_exists_:"author" AND page_count:>200 AND name:/star.*/^1.3

#query
dune god
dune OR god
name:/star.*/ OR name:duna~
_exists_:"author" AND page_count:>200 AND (name:/star.*/ OR name:duna~)
_exists_:"author" AND page_count:>200^1.3 AND (name:/star.*/ OR name:duna~^1.2)^1.2
    )##";

    Map<String, String> column2analyzer;
    String default_field("body");
    QueryDriver driver(column2analyzer, default_field);
    IStringStream iss(row_quires);
    int rc = driver.ParseStream(iss);
    EXPECT_EQ(rc, 0);
}
