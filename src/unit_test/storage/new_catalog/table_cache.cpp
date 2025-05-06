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

#include "gtest/gtest.h"

import base_test;
import stl;
import internal_types;
import catalog_cache;

using namespace infinity;

class TestTableCache : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         TestTableCache,
                         ::testing::Values(BaseTestParamStr::NEW_CONFIG_PATH, BaseTestParamStr::NEW_VFS_OFF_CONFIG_PATH));

TEST_P(TestTableCache, test_table_cache) {
    using namespace infinity;

    {
        UniquePtr<TableCache> table_cache_ptr = MakeUnique<TableCache>(0, 1);
        EXPECT_EQ(table_cache_ptr->table_id(), 1);
        EXPECT_TRUE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        Vector<Pair<RowID, u64>> ranges = table_cache_ptr->PrepareAppendRanges(8190, 2);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8190);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 0);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(ranges.size(), 1);
        EXPECT_EQ(ranges[0].first, RowID(0, 0));
        EXPECT_EQ(ranges[0].second, 8190);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges = table_cache_ptr->PrepareAppendRanges(2, 4);
        EXPECT_EQ(ranges.size(), 1);
        EXPECT_EQ(ranges[0].first, RowID(0, 8190));
        EXPECT_EQ(ranges[0].second, 2);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8192);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 0);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges = table_cache_ptr->PrepareAppendRanges(8190, 6);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 16382);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 0);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(ranges.size(), 1);
        EXPECT_EQ(ranges[0].first, RowID(0, 8192));
        EXPECT_EQ(ranges[0].second, 8190);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges = table_cache_ptr->PrepareAppendRanges(8190, 8);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 24572);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 0);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_EQ(ranges.size(), 1);
        EXPECT_EQ(ranges[0].first, RowID(0, 16382));
        EXPECT_EQ(ranges[0].second, 8190);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(0, 0), 8190));
        table_cache_ptr->CommitAppendRanges(ranges, 2);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 24572);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8190);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 1);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        for (SizeT i = 0; i < 1025; ++i) {
            TransactionID txn_id = 2 * i + 10;
            ranges = table_cache_ptr->PrepareAppendRanges(8192, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 32764);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8190);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 2);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        for (SizeT i = 1024; i < 1024 + 1020; ++i) {
            TransactionID txn_id = 2 * i + 10;
            ranges = table_cache_ptr->PrepareAppendRanges(8192, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8190);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 2);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        Vector<SegmentID> import_segments = table_cache_ptr->GetImportSegments(2);
        Vector<SegmentID> expected_segments;
        expected_segments.emplace_back(2);
        expected_segments.emplace_back(3);
        EXPECT_EQ(import_segments, expected_segments);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);

        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(0, 8190), 2));
        table_cache_ptr->CommitAppendRanges(ranges, 4);
        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(0, 8192), 8190));
        table_cache_ptr->CommitAppendRanges(ranges, 6);
        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(0, 16382), 8190));
        table_cache_ptr->CommitAppendRanges(ranges, 8);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 0);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 24572);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        SegmentOffset offset = 24572;
        SegmentID segment_id = 0;
        for (SizeT i = 0; i < 1025; ++i) {
            TransactionID txn_id = 2 * i + 10;
            ranges.clear();

            if (offset + 8192 > 8388608) {
                u64 len = 8388608 - offset;
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), len));
                ++segment_id;
                offset = 0;
                len = 8192 - len;
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), len));
                offset += len;
            } else {
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), 8192));
                offset += 8192;
            }

            table_cache_ptr->CommitAppendRanges(ranges, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 32764);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        for (SizeT i = 1024; i < 1024 + 1020; ++i) {
            TransactionID txn_id = 2 * i + 10;
            ranges.clear();

            if (offset + 8192 > 8388608) {
                u64 len = 8388608 - offset;
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), len));
                ++segment_id;
                offset = 0;
                len = 8192 - len;
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), len));
                offset += len;
            } else {
                ranges.push_back(Pair<RowID, u64>(RowID(segment_id, offset), 8192));
                offset += 8192;
            }

            table_cache_ptr->CommitAppendRanges(ranges, txn_id);
        }

        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        ranges = table_cache_ptr->PrepareAppendRanges(4, 2 * (1024 + 1020) + 10);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388608);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8388604);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_TRUE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(1, 8388604), 4));
        table_cache_ptr->CommitAppendRanges(ranges, 2 * (1024 + 1020) + 10);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 8388608);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8388608);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 4);
        EXPECT_TRUE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges = table_cache_ptr->PrepareAppendRanges(1, 2 * (1024 + 1021) + 10);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 8388608);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 5);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_TRUE(table_cache_ptr->has_commit_unsealed_segment());

        ranges.clear();
        ranges.push_back(Pair<RowID, u64>(RowID(4, 0), 1));
        table_cache_ptr->CommitAppendRanges(ranges, 2 * (1024 + 1021) + 10);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->prepare_unsealed_segment_offset(), 1);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_id(), 4);
        EXPECT_EQ(table_cache_ptr->commit_unsealed_segment_offset(), 1);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 5);
        EXPECT_FALSE(table_cache_ptr->has_prepared_unsealed_segment());
        EXPECT_FALSE(table_cache_ptr->has_commit_unsealed_segment());

        RowID commit_unsealed_position = table_cache_ptr->GetCommitUnsealedPosition();
        EXPECT_EQ(commit_unsealed_position.segment_id_, 4);
        EXPECT_EQ(commit_unsealed_position.segment_offset_, 1);

        SegmentID compact_segment_id = table_cache_ptr->GetCompactSegment();
        EXPECT_EQ(compact_segment_id, 5);
        EXPECT_EQ(table_cache_ptr->next_segment_id(), 6);
    }
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(0, 0));
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(0, 0));
    //
    //    table_cache_ptr->UpdateCapacityPosition(0, 1);
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(0, 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateCapacityPosition(0, 1024u * 8192u - 1); // last offset of segment
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(0, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateDataPosition(0, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(0, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateCapacityPosition(1, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(1, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateDataPosition(1, 0);
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(1, 0));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateCapacityPosition(2, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(2, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateCapacityPosition(3, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->capacity_position(), RowID(3, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{2});
    //
    //    table_cache_ptr->UpdateDataPosition(1, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(1, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{2});
    //
    //    table_cache_ptr->UpdateDataPosition(2, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(2, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    table_cache_ptr->UpdateDataPosition(3, 1024u * 8192u - 1);
    //    EXPECT_EQ(table_cache_ptr->data_position(), RowID(3, 1024u * 8192u - 1));
    //    EXPECT_EQ(table_cache_ptr->GetDiffSegments(), Deque<SegmentID>{});
    //
    //    EXPECT_ANY_THROW(table_cache_ptr->UpdateCapacityPosition(2, 1024u * 8192u - 1));
    //    EXPECT_ANY_THROW(table_cache_ptr->UpdateCapacityPosition(3, 1024u * 8192u - 2));
    //    EXPECT_ANY_THROW(table_cache_ptr->UpdateDataPosition(2, 1024u * 8192u - 1));
    //    EXPECT_ANY_THROW(table_cache_ptr->UpdateDataPosition(3, 1024u * 8192u - 2));
}
