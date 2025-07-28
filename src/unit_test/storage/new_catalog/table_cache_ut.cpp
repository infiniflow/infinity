// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module;

#include "gtest/gtest.h"

module infinity_core:ut.table_cache;

import :ut.base_test;
import :stl;
import internal_types;
import :catalog_cache;

using namespace infinity;

class TestTableCache : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTableCache,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTableCache, test_table_cache) {
    using namespace infinity;

    {
        UniquePtr<TableCache> table_cache_ptr = MakeUnique<TableCache>(1, "tbl1");
        EXPECT_EQ(table_cache_ptr->table_id(), 1);
        EXPECT_EQ(table_cache_ptr->uncommitted_append_infos_.size(), 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_, nullptr);
        EXPECT_EQ(table_cache_ptr->sealed_segment_cache_map_.size(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 0);

        SharedPtr<AppendPrepareInfo> append_prepare_info = table_cache_ptr->PrepareAppendNolock(8190, 2);
        EXPECT_NE(table_cache_ptr->unsealed_segment_cache_, nullptr);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8190);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(append_prepare_info->ranges_.size(), 1);
        EXPECT_EQ(append_prepare_info->ranges_[0].first, RowID(0, 0));
        EXPECT_EQ(append_prepare_info->ranges_[0].second, 8190);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8190);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 0);

        append_prepare_info = table_cache_ptr->PrepareAppendNolock(2, 4);
        EXPECT_EQ(append_prepare_info->ranges_.size(), 1);
        EXPECT_EQ(append_prepare_info->ranges_[0].first, RowID(0, 8190));
        EXPECT_EQ(append_prepare_info->ranges_[0].second, 2);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8192);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8192);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 0);

        append_prepare_info = table_cache_ptr->PrepareAppendNolock(8190, 6);
        EXPECT_EQ(append_prepare_info->ranges_.size(), 1);
        EXPECT_EQ(append_prepare_info->ranges_[0].first, RowID(0, 8192));
        EXPECT_EQ(append_prepare_info->ranges_[0].second, 8190);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 16382);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 16382);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 0);

        append_prepare_info = table_cache_ptr->PrepareAppendNolock(8190, 8);
        EXPECT_EQ(append_prepare_info->ranges_.size(), 1);
        EXPECT_EQ(append_prepare_info->ranges_[0].first, RowID(0, 16382));
        EXPECT_EQ(append_prepare_info->ranges_[0].second, 8190);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 24572);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 24572);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 0);

        append_prepare_info = table_cache_ptr->uncommitted_append_infos_.front();
        table_cache_ptr->CommitAppendNolock(append_prepare_info, append_prepare_info->transaction_id_);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 24572);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 24572);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 8190);

        for (SizeT i = 0; i < 1022; ++i) {
            TransactionID txn_id = 2 * i + 10;
            table_cache_ptr->PrepareAppendNolock(8192, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8188);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 2);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8188);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 8190);

        for (SizeT i = 1022; i < 1022 + 1023; ++i) {
            TransactionID txn_id = 2 * i + 10;
            table_cache_ptr->PrepareAppendNolock(8192, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8388604);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 2);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8388604);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 8190);

        auto import_info = table_cache_ptr->PrepareImportSegmentsNolock(2, 2045);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->import_prepare_info_map_.size(), 1);
        EXPECT_EQ(import_info->segment_ids_.size(), 2);
        EXPECT_EQ(import_info->row_counts_.size(), 0);
        EXPECT_EQ(import_info->indexes_.size(), 0);

        append_prepare_info = table_cache_ptr->uncommitted_append_infos_.front();
        table_cache_ptr->CommitAppendNolock(append_prepare_info, append_prepare_info->transaction_id_);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8388604);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8388604);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 0);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 8192);

        while (!table_cache_ptr->uncommitted_append_infos_.empty()) {
            append_prepare_info = table_cache_ptr->uncommitted_append_infos_.front();
            table_cache_ptr->CommitAppendNolock(append_prepare_info, append_prepare_info->transaction_id_);
        }

        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->unsealed_segment_cache_->row_count_, 8388604);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->prepare_segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->prepare_segment_offset_, 8388604);
        EXPECT_EQ(table_cache_ptr->commit_segment_id_, 1);
        EXPECT_EQ(table_cache_ptr->commit_segment_offset_, 8388604);

        import_info->row_counts_.push_back(8190);
        import_info->row_counts_.push_back(8190 * 1024);
        table_cache_ptr->CommitImportSegmentsNolock(import_info, 2045);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->import_prepare_info_map_.size(), 0);
    }
}
