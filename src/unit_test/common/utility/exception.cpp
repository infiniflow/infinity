//
// Created by JinHai on 2022/9/4.
//

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