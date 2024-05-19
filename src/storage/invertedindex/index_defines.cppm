module;

export module index_defines;

import stl;

import default_values;
import internal_types;

namespace infinity {

export {

    typedef u8 optionflag_t;

    enum OptionFlag {
        of_none = 0,
        of_term_payload = 1,   // 1 << 0
        of_doc_payload = 2,    // 1 << 1
        of_position_list = 4,  // 1 << 2
        of_term_frequency = 8, // 1 << 3
        of_block_max = 16,     // 1 << 4
    };

    typedef u16 docpayload_t;
    typedef u32 termpayload_t;
    typedef u8 optionflag_t;
    typedef u32 docid_t;
    typedef u32 pos_t;
    typedef u32 df_t;
    typedef u32 tf_t;
    typedef i64 ttf_t;

    constexpr optionflag_t OPTION_FLAG_ALL = of_term_payload | of_doc_payload | of_position_list | of_term_frequency | of_block_max;
    constexpr optionflag_t NO_BLOCK_MAX = of_term_payload | of_doc_payload | of_position_list | of_term_frequency;
    constexpr optionflag_t NO_TERM_FREQUENCY = of_term_payload | of_doc_payload;
    constexpr optionflag_t OPTION_FLAG_NONE = of_none;
    constexpr docid_t INVALID_DOCID = -1;
    constexpr RowID INVALID_ROWID = u64(-1);
    constexpr pos_t INVALID_POSITION = std::numeric_limits<u32>::max();
    constexpr u32 MAX_DOC_PER_RECORD = 128;
    constexpr u32 MAX_DOC_PER_RECORD_BIT_NUM = 7;
    constexpr u32 MAX_DOC_PER_BITMAP_BLOCK = 256;
    constexpr u32 MAX_DOC_PER_BITMAP_BLOCK_BIT_NUM = 8;
    constexpr u32 MAX_POS_PER_RECORD = 128;
    constexpr u32 MAX_POS_PER_RECORD_BIT_NUM = 7;
    constexpr u32 MAX_POS_PER_RECORD_MASK = MAX_POS_PER_RECORD - 1;
    constexpr u32 MAX_UNCOMPRESSED_DOC_LIST_SIZE = 5;
    constexpr u32 MAX_UNCOMPRESSED_POS_LIST_SIZE = 5;
    constexpr u32 MAX_UNCOMPRESSED_SKIP_LIST_SIZE = 10;
    constexpr u8 SKIP_LIST_BUFFER_SIZE = 32;

    constexpr const char *DICT_SUFFIX = ".dic";
    constexpr const char *POSTING_SUFFIX = ".pos";
    constexpr const char *SPILL_SUFFIX = ".spill";
    constexpr const char *LENGTH_SUFFIX = ".len";

    using ScoredId = Pair<float, u32>;
    using ScoredIds = Vector<ScoredId>;
    constexpr std::string_view DEFAULT_SCORER = "bm25";
    constexpr std::string_view DEFAULT_SCORER_ARG = "";
    constexpr SizeT DEFAULT_TOPN = 100;
}

// iresearch doc_id begins with 1
export u32 RowID2DocID(const RowID &row_id) { return (row_id.segment_id_ << SEGMENT_OFFSET_IN_DOCID) + row_id.segment_offset_ + 1; }

export u32 RowID2DocID(u32 segment_id, u32 block_id, u32 block_offset) {
    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + block_offset;
    return (segment_id << SEGMENT_OFFSET_IN_DOCID) + segment_offset + 1;
}

export RowID DocID2RowID(u32 doc_id) { return RowID((doc_id - 1) >> SEGMENT_OFFSET_IN_DOCID, (doc_id - 1) & SEGMENT_MASK_IN_DOCID); }

} // namespace infinity
