#pragma once

#include "common/types/internal_types.h"
#include <cstdlib>

namespace infinity {
namespace {
String available_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

inline String
RandomString(SizeT len, u32 seed = 0) {
    srand(seed);
    String ret(len, '\0');
    for(SizeT i = 0; i < len; i++) {
        SizeT rand_i = random() % available_chars.size();
        ret[i] = available_chars[rand_i];
    }
    return ret;
}
}