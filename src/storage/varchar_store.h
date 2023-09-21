#pragma once

#include "common/types/data_type.h"
#include "common/types/internal_types.h"
namespace infinity {
// TODO shenyushi: move to a seperate file.

#pragma pack (1)

struct VarcharStore {
    u16 length_{};

    struct ShortInfo {
        Array<char_t, VarcharT::INLINE_LENGTH> data{};
    };

    struct LongInfo {
        Array<char_t, VarcharT::PREFIX_LENGTH> prefix_{};

        u16 file_idx_{};

        u16 _gap_{};

        u32 file_offset_{};
    };

    static_assert(sizeof(ShortInfo) == sizeof(LongInfo));

    union {
        ShortInfo short_info_;
        LongInfo long_info_;
    } u;
};

static_assert(sizeof(VarcharStore) == sizeof(VarcharT));

#pragma pack ()

}