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

export module inmem_index_segment_reader;

import stl;

import segment_posting;
import index_segment_reader;
import index_defines;
import posting_writer;
import memory_indexer;
import internal_types;

namespace infinity {
export class InMemIndexSegmentReader : public IndexSegmentReader {
public:
    InMemIndexSegmentReader(MemoryIndexer *memory_indexer);
    virtual ~InMemIndexSegmentReader() = default;

    bool GetSegmentPosting(const String &term, SegmentPosting &seg_posting, bool fetch_position = true) const override;

private:
    SharedPtr<MemoryIndexer::PostingTable> posting_table_;
    RowID base_row_id_{INVALID_ROWID};
};

} // namespace infinity