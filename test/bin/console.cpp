//
// Created by JinHai on 2022/10/1.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/logger.h"

class ConsoleTest : public BaseTest {

};

TEST_F(ConsoleTest, test1) {
    std::cout << "Console Test" << std::endl;
}