export module infinity_core:short_list_optimize_util;

import :index_defines;

namespace infinity {
export class ShortListOptimizeUtil {
public:
    ShortListOptimizeUtil();
    ~ShortListOptimizeUtil() = default;

    static bool IsShortDocList(df_t df) { return df <= (df_t)MAX_UNCOMPRESSED_DOC_LIST_SIZE; }

    static bool IsShortPosList(ttf_t ttf) { return ttf <= (ttf_t)MAX_UNCOMPRESSED_POS_LIST_SIZE; }

    static bool IsShortSkipList(u32 size) { return size <= MAX_UNCOMPRESSED_SKIP_LIST_SIZE; }
};
} // namespace infinity