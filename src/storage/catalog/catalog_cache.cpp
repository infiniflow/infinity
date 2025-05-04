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

#include <vector>

module catalog_cache;

import stl;
import internal_types;
import default_values;
import infinity_exception;
import third_party;


namespace infinity {

Vector<Pair<RowID, u64>> TableCache::PrepareAppendRanges(SizeT row_count, TransactionID transaction_id) {
    if (row_count > MAX_BLOCK_CAPACITY) {
        UnrecoverableError(fmt::format("Attempt to prepare row_count: {} > MAX_BLOCK_CAPACITY: {}", row_count, MAX_BLOCK_CAPACITY));
    }

    Vector<Pair<RowID, u64>> ranges;
    if (has_prepare_unsealed_segment_) {
        prepare_unsealed_segment_id_ = next_segment_id_;
        prepare_unsealed_segment_offset_ = row_count;
        ++next_segment_id_;
        ranges.emplace_back(RowID(prepare_unsealed_segment_id_, 0), row_count);
        has_prepare_unsealed_segment_ = false;
    } else {
        if (prepare_unsealed_segment_offset_ + row_count < DEFAULT_SEGMENT_CAPACITY) {
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), row_count);
            prepare_unsealed_segment_offset_ += row_count;
            has_prepare_unsealed_segment_ = false;
        } else if (prepare_unsealed_segment_offset_ + row_count == DEFAULT_SEGMENT_CAPACITY) {
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), row_count);
            prepare_unsealed_segment_offset_ += row_count;
            has_prepare_unsealed_segment_ = true;
        } else {
            SizeT remaining = DEFAULT_SEGMENT_CAPACITY - prepare_unsealed_segment_offset_;
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), remaining);
            prepare_unsealed_segment_id_ = next_segment_id_;
            ++next_segment_id_;
            prepare_unsealed_segment_offset_ = row_count - remaining; // remaining rows must not exceed DEFAULT_SEGMENT_CAPACITY
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, 0), prepare_unsealed_segment_offset_);
            has_prepare_unsealed_segment_ = false;
        }
    }

    // append all ranges item to prepared_append_ranges_
    for (const auto &range : ranges) {
        prepared_append_ranges_.emplace_back(range.first, range.second, transaction_id);
    }
    return ranges;
}

void TableCache::CommitAppendRanges(const Vector<Pair<RowID, u64>> &ranges, TransactionID txn_id) {
    for (const auto &range : ranges) {
        auto [row_offset, row_count, transaction] = prepared_append_ranges_.front();
        if (range.first != row_offset or range.second != row_count or transaction != txn_id) {
            UnrecoverableError(fmt::format("Attempt to commit row range: {}: {}, {} != {}: {}, {}",
                                           txn_id,
                                           range.first.ToString(),
                                           range.second,
                                           transaction,
                                           row_offset.ToString(),
                                           row_count));
        }
        prepared_append_ranges_.pop_front();
        commit_unsealed_segment_id_ = range.first.segment_id_;
        commit_unsealed_segment_offset_ += range.second;
        if (commit_unsealed_segment_offset_ == DEFAULT_SEGMENT_CAPACITY) {
            has_commit_unsealed_segment_ = true;
        } else if (commit_unsealed_segment_offset_ > DEFAULT_SEGMENT_CAPACITY) {
            commit_unsealed_segment_offset_ -= DEFAULT_SEGMENT_CAPACITY;
            has_commit_unsealed_segment_ = false;
        } else {
            has_commit_unsealed_segment_ = false;
        }
    }
}

RowID TableCache::GetCommitUnsealedPosition() {
    if (has_commit_unsealed_segment_) {
        UnrecoverableError("No unsealed segment");
    }
    return RowID(commit_unsealed_segment_id_, commit_unsealed_segment_offset_);
}

Vector<SegmentID> TableCache::GetImportSegments(SizeT segment_count) {
    Vector<SegmentID> segment_ids;
    segment_ids.reserve(segment_count);
    for (SizeT i = 0; i < segment_count; ++i) {
        segment_ids.emplace_back(next_segment_id_);
        ++next_segment_id_;
    }
    return segment_ids;
}

SegmentID TableCache::GetCompactSegment() {
    // For debug, not to use next_segment_id_++
    SegmentID segment_id = next_segment_id_;
    next_segment_id_ += 1;
    return segment_id;
}

Pair<RowID, u64> TableCache::PrepareDumpIndexRange(u64 index_id) { return {RowID(), 0}; }
} // namespace infinity