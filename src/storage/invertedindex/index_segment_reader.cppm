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

import segment_posting;
import index_defines;
export module index_segment_reader;

namespace infinity {
export class IndexSegmentReader {
public:
    IndexSegmentReader(SegmentID segment_id, ChunkID chunk_id);
    virtual ~IndexSegmentReader() {}

    // fetch_position is only valid in DiskIndexSegmentReader
    virtual bool GetSegmentPosting(const String &term, SegmentPosting &seg_posting, bool fetch_position = true) const = 0;

    SegmentID segment_id() const { return segment_id_; }

    ChunkID chunk_id() const { return chunk_id_; }

private:
    SegmentID segment_id_;
    ChunkID chunk_id_;
};

} // namespace infinity