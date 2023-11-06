//
// Created by jinhai on 23-8-19.
//

#include "unit_test/base_test.h"
#include <string>

import infinity;
import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;
import value;
import default_values;
import base_entry;
import block_column_entry;
import block_entry;

class BlockVersionTest : public BaseTest {};

TEST_F(BlockVersionTest, SaveAndLoad) {
    using namespace infinity;
    BlockVersion block_version(8192);
    block_version.created_.emplace_back(10, 3);
    block_version.created_.emplace_back(20, 6);
    block_version.deleted_[2] = 30;
    block_version.deleted_[5] = 40;
    String version_path("/tmp/block_version_test");
    block_version.SaveToFile(version_path);

    BlockVersion block_verson2(8192);
    block_verson2.LoadFromFile(version_path);
    ASSERT_EQ(block_version, block_verson2);
}
