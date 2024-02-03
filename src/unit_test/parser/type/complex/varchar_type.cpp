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

import infinity_exception;

import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import data_type;
import parser_assert;

class VarcharTypeTest : public BaseTest {};

TEST_F(VarcharTypeTest, TestInit) {
    using namespace infinity;
    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";

    // Three InitAsValue function
    {
        VarcharT v1;
        v1.InitAsValue(s1);
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        v1.InitAsValue(s2);
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        v1.Reset();

        v1.InitAsValue(s1.c_str());
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        v1.InitAsValue(s2.c_str());
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        v1.Reset();

        v1.InitAsValue(s1.c_str(), s1.length());
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        v1.InitAsValue(s2.c_str(), s2.length());
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        v1.Reset();

        v1.InitAsValue(s2.c_str(), s2.length());
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        EXPECT_THROW(v1.InitAsValue(s1.c_str(), s1.length()), ParserException);
        v1.Reset();
    }
}

TEST_F(VarcharTypeTest, TestConstructor1) {
    using namespace infinity;
    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";

    // Constructor from string
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        VarcharT v2;
        v2.InitAsValue(s2);
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
        v2.Reset();
    }
}

TEST_F(VarcharTypeTest, TestConstructor2) {
    using namespace infinity;
    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";

    {

        VarcharT v1;
        v1.InitAsValue(s1);
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        VarcharT v2;
        v2.InitAsValue(s2);
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
        v2.Reset();
    }
}

TEST_F(VarcharTypeTest, TestConstructor3) {
    using namespace infinity;

    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";

    {

        VarcharT v1;
        v1.InitAsValue(s1.c_str(), s1.length());
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        v1.Reset();

        VarcharT v2;
        v2.InitAsValue(s2.c_str(), s2.length());
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
        v2.Reset();
    }
}

TEST_F(VarcharTypeTest, TestCopy) {
    using namespace infinity;

    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";

    // Copy constructor
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        VarcharT v2 = v1;
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        EXPECT_EQ(v2.length_, s1.length());
        EXPECT_EQ(v2.ToString(), s1);
    }

    {

        VarcharT v1;
        v1.InitAsValue(s2);
        VarcharT v2 = v1;
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
    }
}

//TEST_F(VarcharTypeTest, TestMove) {
//    using namespace infinity;
//
//    String s1 = "Hello World";
//    String s2 = "Hello World, welcome to c++";
//
//    // std::move constructor 1
//    {
//
//        VarcharT v1;
//        v1.InitAsValue(s1);
//        VarcharT v2 = std::move(v1);
//        EXPECT_EQ(v1.length_, 0);
//        EXPECT_EQ(v1.ToString(), "");
//        EXPECT_EQ(v2.length_, s1.length());
//        EXPECT_EQ(v2.ToString(), s1);
//    }
//
//    // std::move constructor 2
//    {
//
//        VarcharT v1;
//        v1.InitAsValue(s2);
//        VarcharT v2 = std::move(v1);
//        EXPECT_EQ(v1.length_, 0);
//        EXPECT_EQ(v1.ptr, nullptr);
//        EXPECT_EQ(v1.ToString(), "");
//        EXPECT_EQ(v2.length_, s2.length());
//        EXPECT_EQ(v2.ToString(), s2);
//    }
//}

TEST_F(VarcharTypeTest, TestCopyAssignment) {
    using namespace infinity;

    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";
    String s3 = "Hello World, Php is the best programming language in the world";

    // Copy Assignment 1
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        VarcharT v2;
        v2 = v1;
        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        EXPECT_EQ(v2.length_, s1.length());
        EXPECT_EQ(v2.ToString(), s1);
    }

    // Copy Assignment 2
    {

        VarcharT v1;
        v1.InitAsValue(s2);
        VarcharT v2;
        v2 = v1;
        EXPECT_EQ(v1.length_, s2.length());
        EXPECT_EQ(v1.ToString(), s2);
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
    }

    // Copy Assignment 3
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        VarcharT v2;
        v2.InitAsValue(s2);
        VarcharT v3;
        v3.InitAsValue(s3);

        v2 = v1;

        EXPECT_EQ(v1.length_, s1.length());
        EXPECT_EQ(v1.ToString(), s1);
        EXPECT_EQ(v2.length_, s1.length());
        EXPECT_EQ(v2.ToString(), s1);

        v2 = v3;
        EXPECT_EQ(v2.length_, s3.length());
        EXPECT_EQ(v2.ToString(), s3);
    }
}

TEST_F(VarcharTypeTest, TestMoveAssignment) {
    using namespace infinity;

    String s1 = "Hello World";
    String s2 = "Hello World, welcome to c++";
    String s3 = "Hello World, Php is the best programming language in the world";

    // std::move constructor 1
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        VarcharT v2;
        v2 = std::move(v1);
        EXPECT_EQ(v1.length_, 0);
        EXPECT_EQ(v1.ToString(), "");
        EXPECT_EQ(v2.length_, s1.length());
        EXPECT_EQ(v2.ToString(), s1);
    }

    // std::move constructor 2
    {

        VarcharT v1;
        v1.InitAsValue(s2);
        VarcharT v2;
        v2 = std::move(v1);
        EXPECT_EQ(v1.length_, 0);
        EXPECT_EQ(v1.value_.ptr_, nullptr);
        EXPECT_EQ(v1.ToString(), "");
        EXPECT_EQ(v2.length_, s2.length());
        EXPECT_EQ(v2.ToString(), s2);
    }

    // std::move Assignment 3
    {

        VarcharT v1;
        v1.InitAsValue(s1);
        VarcharT v2;
        v2.InitAsValue(s2);
        VarcharT v3;
        v3.InitAsValue(s3);

        v2 = std::move(v1);

        EXPECT_EQ(v1.length_, 0);
        EXPECT_EQ(v1.ToString(), "");
        EXPECT_EQ(v2.length_, s1.length());
        EXPECT_EQ(v2.ToString(), s1);

        v2 = std::move(v3);
        EXPECT_EQ(v2.length_, s3.length());
        EXPECT_EQ(v2.ToString(), s3);
        EXPECT_EQ(v3.length_, 0);
        EXPECT_EQ(v3.value_.ptr_, nullptr);
    }
}
