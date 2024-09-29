#include "gtest/gtest.h"
import base_test;

import infinity;
import infinity_context;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import catalog;
import base_entry;
import status;
import logger;
import third_party;
import cleanup_scanner;

using namespace infinity;

class CleanUpScannerTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         CleanUpScannerTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(CleanUpScannerTest, nonexist_dir_test) {
    String dir = "/nonexist_dir";
    EXPECT_NO_THROW(CleanupScanner::CleanupDir(dir));
}