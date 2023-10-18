//
// Created by JinHai on 2022/11/28.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import logger;
import column_vector;
import value;
import parser;
import default_values;
import third_party;
import stl;
import selection;
import vector_buffer;

class ColumnVectorNestedTest : public BaseTest {
    void SetUp() override {
    }

    void TearDown() override {
    }
};
TEST_F(ColumnVectorNestedTest, flat_array) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, contant_array) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, flat_tuple) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, contant_tuple) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    // No test cases.
}
