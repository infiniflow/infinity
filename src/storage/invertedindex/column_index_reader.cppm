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
import memory_pool;
import third_party;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import disk_index_segment_reader;
import inmem_index_segment_reader;
export module column_index_reader;

namespace infinity {

export class ColumnIndexReader {
public:
    ColumnIndexReader();
    virtual ~ColumnIndexReader() = default;

    void Open(const String &index_dir, const Vector<String> &base_names, const Vector<docid_t> &base_docids, optionflag_t flag);

    PostingIterator *Lookup(const String &term, MemoryPool *session_pool);

    bool GetSegmentPosting(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool) { return false; }

private:
    SharedPtr<DiskIndexSegmentReader>
    CreateDiskSegmentReader(const String &index_dir, const String &base_name, docid_t base_doc_id, optionflag_t flag);

private:
    optionflag_t flag_;
    Vector<SharedPtr<IndexSegmentReader>> segment_readers_;
    Vector<docid_t> base_doc_ids_;
};

namespace detail {
template <class T>
struct Hash {
    inline SizeT operator()(const T &val) const { return val; }
};
} // namespace detail

export struct IndexReader {
    ColumnIndexReader *GetColumnIndexReader(u64 column_id) { return column_index_readers_[column_id].get(); }

    FlatHashMap<u64, UniquePtr<ColumnIndexReader>, detail::Hash<u64>> column_index_readers_;
    SharedPtr<MemoryPool> session_pool_;
};

} // namespace infinity