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

module infinity_core:ut.logical_plan_tpch;

import :ut.base_test;
#endif

#if 0
using namespace infinity;
class LogicalPlannerTpchTest : public BaseTest {};

TEST_F(LogicalPlannerTpchTest, test1) {
    using namespace infinity;

    // Create tables;
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/create.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Show tables
    {
        SQLRunner::Run("show tables;", false);
    }

    // Insert CUSTOMER table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_customer.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }
    // Insert LINEITEM table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_lineitem.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }
    // Insert NATION table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_nation.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Insert ORDERS table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_orders.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Insert PARTS table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_parts.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Insert PARTSUPP table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_partsupp.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Insert REGION table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_region.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    // Insert SUPPLIER table
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/insert_supplier.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }

    {
        // Q1
        std::string sql_text = std::string(TEST_DATA_PATH) + "/tpch/q01.sql";
        std::ifstream t(sql_text.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    }



    {
        // Q6
        std::string sql_text = std::string(TEST_DATA_PATH) + "/tpch/q06.sql";
        std::ifstream t(sql_text.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    }


    // DROP tables;
    {
        std::string ddl_sql = std::string(TEST_DATA_PATH) + "/tpch/drop.sql";
        std::ifstream t(ddl_sql.c_str());
        std::string input_sql((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        SQLRunner::Run(input_sql, false);
    }
}

#endif
