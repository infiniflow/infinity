module;

import stl;

export module index_defines;

namespace infinity {

export {

    typedef u8 optionflag_t;
    typedef u8 token_len_t;

    enum HighFrequencyTermPostingType { hp_bitmap, hp_both };

    enum PostingIteratorType {
        pi_pk,
        pi_bitmap,
        pi_buffered,
        pi_range,
        pi_seek_and_filter,
        pi_dynamic,
        pi_composite,
        pi_unknown,
    };

    enum OptionFlag {
        of_none = 0,
        of_term_payload = 1,     // 1 << 0
        of_doc_payload = 2,      // 1 << 1
        of_position_payload = 4, // 1 << 2
        of_position_list = 8,    // 1 << 3
        of_term_frequency = 16,  // 1 << 4
        of_tf_bitmap = 32,       // 1 << 5
    };

    enum PostingType {
        pt_default,
        pt_normal,
        pt_bitmap,
    };

    typedef u16 docpayload_t;
    typedef u32 termpayload_t;
    typedef u8 optionflag_t;
    typedef u32 docid_t;
    typedef u32 pos_t;
    typedef u32 df_t;
    typedef u32 tf_t;
    typedef i64 ttf_t;
    typedef u32 field_len_t;

    constexpr optionflag_t OPTION_FLAG_ALL = of_term_payload | of_doc_payload | of_position_list | of_term_frequency;
    constexpr optionflag_t NO_TERM_FREQUENCY = of_doc_payload | of_term_payload;
    constexpr optionflag_t OPTION_FLAG_NONE = of_none;
    constexpr docid_t INVALID_DOCID = -1;
    constexpr pos_t INVALID_POSITION = u32_max;
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
}

} // namespace infinity
