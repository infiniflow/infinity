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

module;

import stl;
import hnsw_common;
import bitmask;
import catalog;

export module knn_filter;

namespace infinity {

export template <typename LabelType>
class BitmaskFilter final : public FilterBase<LabelType> {
public:
    explicit BitmaskFilter(const Bitmask &bitmask) : bitmask_(bitmask) {}

    bool operator()(const LabelType &label) const final { return bitmask_.IsTrue(label); }

private:
    const Bitmask &bitmask_;
};

export class DeleteFilter final : public FilterBase<SegmentOffset> {
public:
    explicit DeleteFilter(const SegmentEntry *segment, TxnTimeStamp query_ts) : segment_(segment), query_ts_(query_ts) {}

    bool operator()(const SegmentOffset &segment_offset) const final { return segment_->CheckVisible(segment_offset, query_ts_); }

private:
    const SegmentEntry *const segment_;

    const TxnTimeStamp query_ts_;
};

export class DeleteWithBitmaskFilter final : public FilterBase<SegmentOffset> {
public:
    explicit DeleteWithBitmaskFilter(const Bitmask &bitmask, const SegmentEntry *segment, TxnTimeStamp query_ts)
        : bitmask_filter_(bitmask), delete_filter_(segment, query_ts) {}

    bool operator()(const SegmentOffset &segment_offset) const final { return bitmask_filter_(segment_offset) && delete_filter_(segment_offset); }

private:
    BitmaskFilter<SegmentOffset> bitmask_filter_;
    DeleteFilter delete_filter_;
};

} // namespace infinity
