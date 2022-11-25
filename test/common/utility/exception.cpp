//
// Created by JinHai on 2022/9/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/utility/exception.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ExceptionTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ExceptionTest, test1) {
    try {
        throw infinity::CatalogException("Can't find");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Catalog Error: Can't find");
    }

    try {
        throw infinity::NetworkException("Can't connected");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Network Error: Can't connected");
    }

    try {
        throw infinity::ParserException("Unsupported syntax");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Parser Error: Unsupported syntax");
    }

    try {
        throw infinity::PlannerException("Doesn't support this usage:", "SELECT *");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Planner Error: Doesn't support this usage: SELECT *");
    }

    try {
        throw infinity::OptimizerException("Unsupported syntax");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Optimizer Error: Unsupported syntax");
    }

    try {
        throw infinity::ExecutorException("Can't execute:", "out of memory");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Executor Error: Can't execute: out of memory");
    }

    try {
        throw infinity::SchedulerException("Schedule the task:", "timeout,", "need to restart");
    } catch (std::exception& e) {
        EXPECT_EQ(std::string(e.what()), "Scheduler Error: Schedule the task: timeout, need to restart");
    }

}