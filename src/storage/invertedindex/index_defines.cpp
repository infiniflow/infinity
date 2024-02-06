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
