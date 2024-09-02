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

#include "gtest/gtest.h"
import base_test;

import stl;
import segment_entry;
import DBT_compaction_alg;

using namespace infinity;

class MockSegmentEntry : public SegmentEntry {
    static SegmentID cur_segment_id_;

public:
    static constexpr SegmentOffset segment_capacity = 1000;

    static SharedPtr<MockSegmentEntry> Make(SizeT row_cnt) { return MakeShared<MockSegmentEntry>(cur_segment_id_++, row_cnt); }

    MockSegmentEntry(SegmentID segment_id, SizeT row_cnt) : SegmentEntry(nullptr, nullptr, segment_id, 0, 0, SegmentStatus::kSealed) {
        this->IncreaseRowCount(row_cnt);
    }

    static Vector<SharedPtr<MockSegmentEntry>> MockCompact(const Vector<SegmentEntry *> &segments) {
        SegmentOffset sum_row_cnt = 0;
        Vector<SharedPtr<MockSegmentEntry>> ret;
        for (const auto *segment : segments) {
            SegmentOffset cnt = segment->actual_row_count();
            if (sum_row_cnt + cnt > segment_capacity) {
                ret.emplace_back(MockSegmentEntry::Make(sum_row_cnt));
                sum_row_cnt = 0;
            }
            sum_row_cnt += cnt;
        }
        if (sum_row_cnt > 0) {
            ret.emplace_back(MockSegmentEntry::Make(sum_row_cnt));
        }
        return ret;
    }

    void ShrinkSegment(SizeT row_cnt) { this->DecreaseRemainRow(row_cnt); }
};

SegmentID MockSegmentEntry::cur_segment_id_ = 0;

class DBTCompactionTest : public BaseTest {};

TEST_F(DBTCompactionTest, AddSegments) {
    TransactionID txn_id = 0;

    int m = 3;
    int c = 3;
    int s = 1;
    DBTCompactionAlg DBTCompact(m, c, s, MockSegmentEntry::segment_capacity);
    DBTCompact.Enable(Vector<SegmentEntry *>{});

    // {1, 2, 2, 3, 5, 3, 6};
    Vector<SharedPtr<SegmentEntry>> segment_entries; // hold lifetime
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(2);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(2);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);

        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 5u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(3);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(5);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 13u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(3);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(6);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(9);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(11);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 33u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(100);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
}

TEST_F(DBTCompactionTest, AddAndDeleteInSegments) {
    TransactionID txn_id = 0;

    int m = 3;
    int c = 3;
    int s = 1;
    DBTCompactionAlg DBTCompact(m, c, s, MockSegmentEntry::segment_capacity);
    DBTCompact.Enable(Vector<SegmentEntry *>{});

    Vector<SharedPtr<SegmentEntry>> segment_entries; // hold lifetime
    MockSegmentEntry *new_segment = nullptr;         // the segment in layer[1]
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        new_segment = static_cast<MockSegmentEntry *>(compacted_segments[0].get());
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 3u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        new_segment->ShrinkSegment(2);
        DBTCompact.DeleteInSegment(new_segment->segment_id());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        new_segment = static_cast<MockSegmentEntry *>(compacted_segments[0].get());
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 3u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(1);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        new_segment->ShrinkSegment(1);
        DBTCompact.DeleteInSegment(new_segment->segment_id());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 4u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
}

TEST_F(DBTCompactionTest, FillSegmentCapacity) {
    TransactionID txn_id = 0;

    int m = 4;
    int c = 10;
    int s = 10;
    // layer0: 0~9, layer1: 10~99, layer2: 100~999
    DBTCompactionAlg DBTCompact(m, c, s, MockSegmentEntry::segment_capacity);
    DBTCompact.Enable(Vector<SegmentEntry *>{});

    Vector<SharedPtr<SegmentEntry>> segment_entries; // hold lifetime
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 4u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 800u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(200);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
}

// add rollback test, rollback every compact operation once before success commit it
TEST_F(DBTCompactionTest, RollbackTest) {
    TransactionID txn_id = 0;

    int m = 3;
    int c = 3;
    int s = 1;
    DBTCompactionAlg DBTCompact(m, c, s, MockSegmentEntry::segment_capacity);
    DBTCompact.Enable(Vector<SegmentEntry *>{});

    Vector<SharedPtr<SegmentEntry>> segment_entries; // hold lifetime
    MockSegmentEntry *shrink_segment = nullptr;      // the segment in layer[1]
    {
        auto segment_entry = MockSegmentEntry::Make(2);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(2);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(2);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        DBTCompact.RollbackCompact(txn_id);
    }
    {
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 6u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(8);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(6);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        DBTCompact.RollbackCompact(txn_id);
    }
    {
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 20u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(3);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(6);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(14);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        auto segment_entry = MockSegmentEntry::Make(17);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        DBTCompact.RollbackCompact(txn_id);
    }
    {
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        EXPECT_EQ(compacted_segments.size(), 1u);
        shrink_segment = compacted_segments[0].get();
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 51u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
    {
        auto segment_entry = MockSegmentEntry::Make(100);
        segment_entries.emplace_back(segment_entry);
        DBTCompact.AddSegment(segment_entry.get());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_TRUE(segments.empty());
    }
    {
        shrink_segment->ShrinkSegment(48);
        DBTCompact.DeleteInSegment(shrink_segment->segment_id());
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        DBTCompact.RollbackCompact(txn_id);
    }
    {
        auto segments = DBTCompact.CheckCompaction(++txn_id);
        EXPECT_FALSE(segments.empty());
        EXPECT_EQ(segments.size(), 3u);
        auto compacted_segments = MockSegmentEntry::MockCompact(segments);
        segment_entries.insert(segment_entries.end(), compacted_segments.begin(), compacted_segments.end());
        EXPECT_EQ(compacted_segments.size(), 1u);
        EXPECT_EQ(compacted_segments[0]->actual_row_count(), 12u);

        DBTCompact.CommitCompact(txn_id);
        for (auto &segment : compacted_segments) {
            DBTCompact.AddSegment(segment.get());
            auto segments = DBTCompact.CheckCompaction(++txn_id);
            EXPECT_TRUE(segments.empty());
        }
    }
}
