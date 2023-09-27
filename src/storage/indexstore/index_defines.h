#pragma once

#include <cstdint>

namespace infinity {

typedef uint8_t optionflag_t;
typedef uint8_t token_len_t;

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

typedef uint16_t docpayload_t;
typedef uint8_t pospayload_t;
typedef uint32_t termpayload_t;
typedef uint8_t optionflag_t;
typedef uint32_t docid_t;
typedef uint32_t pos_t;
typedef uint32_t df_t;
typedef uint32_t tf_t;
typedef int64_t ttf_t;
typedef uint32_t field_len_t;

constexpr optionflag_t OPTION_FLAG_ALL = of_term_payload | of_doc_payload | of_position_payload | of_position_list | of_term_frequency;
constexpr uint32_t MAX_DOC_PER_RECORD = 128;
constexpr uint8_t SKIP_LIST_BUFFER_SIZE = 32;

}
