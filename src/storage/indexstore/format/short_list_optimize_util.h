#pragma once

#include "storage/indexstore/index_defines.h"

namespace infinity {
class ShortListOptimizeUtil {
public:
    ShortListOptimizeUtil();
    ~ShortListOptimizeUtil() = default;

    static bool
    IsShortDocList(df_t df) { return df <= (df_t)MAX_UNCOMPRESSED_DOC_LIST_SIZE; }

    static bool
    IsShortPosList(ttf_t ttf) { return ttf <= (ttf_t)MAX_UNCOMPRESSED_POS_LIST_SIZE; }

    static bool
    IsShortSkipList(uint32_t size) { return size <= MAX_UNCOMPRESSED_SKIP_LIST_SIZE; }
};
}// namespace infinity