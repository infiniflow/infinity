//
// Created by JinHai on 2022/9/30.
//

#include "infinity_test.h"
#include "main/infinity.h"

class LoggerTest : public InfinityTest {};

TEST_F(LoggerTest, test1) {
    LOG_TRACE("This is a trace log");
    LOG_INFO("This is a info log");
    LOG_WARN("This is a warn log");
    LOG_ERROR("This is a error log");
    LOG_CRITICAL("This is a error log");
}