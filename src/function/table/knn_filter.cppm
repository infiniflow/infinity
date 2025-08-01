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

export module infinity_core:knn_filter;

import :stl;
import :hnsw_common;
import :roaring_bitmap;

namespace infinity {

export template <typename LabelType>
class BitmaskFilter final : public FilterBase<LabelType> {
public:
    explicit BitmaskFilter(const Bitmask &bitmask) : bitmask_(bitmask) {}

    bool operator()(const LabelType &label) const final { return bitmask_.IsTrue(label); }

private:
    const Bitmask &bitmask_;
};

export class AppendFilter final : public FilterBase<SegmentOffset> {
public:
    AppendFilter(SegmentOffset max_segment_offset) : max_segment_offset_(max_segment_offset) {}

    bool operator()(const SegmentOffset &segment_offset) const final { return segment_offset < max_segment_offset_; }

private:
    const SegmentOffset max_segment_offset_;
};

} // namespace infinity
