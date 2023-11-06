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

class ExceptionTest : public BaseTest {};

TEST_F(ExceptionTest, all_exception_cases) {
    using namespace infinity;

    try {
        throw infinity::CatalogException("Can't find");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Catalog Error: Can't find");
    }

    try {
        throw infinity::NetworkException("Can't connected");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Network Error: Can't connected");
    }

    try {
        throw infinity::PlannerException("Doesn't support this usage:", "SELECT *");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Planner Error: Doesn't support this usage: SELECT *");
    }

    try {
        throw infinity::OptimizerException("Unsupported syntax");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Optimizer Error: Unsupported syntax");
    }

    try {
        throw infinity::ExecutorException("Can't execute:", "out of memory");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Executor Error: Can't execute: out of memory");
    }

    try {
        throw infinity::SchedulerException("Schedule the task:", "timeout,", "need to restart");
    } catch (std::exception &e) {
        EXPECT_EQ(std::string(e.what()), "Scheduler Error: Schedule the task: timeout, need to restart");
    }
}