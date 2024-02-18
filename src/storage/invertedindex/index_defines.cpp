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

import default_values;

module index_defines;

namespace infinity {

docid_t RowID2DocID(const RowID &row_id) { return (row_id.segment_id_ << SEGMENT_OFFSET_IN_DOCID) + row_id.segment_offset_; }

docid_t RowID2DocID(u32 segment_id, u32 block_id, u32 block_offset) {
    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + block_offset;
    return (segment_id << SEGMENT_OFFSET_IN_DOCID) + segment_offset;
}

RowID DocID2RowID(docid_t doc_id) { return RowID(doc_id >> SEGMENT_OFFSET_IN_DOCID, doc_id & SEGMENT_MASK_IN_DOCID); }

} // namespace infinity
