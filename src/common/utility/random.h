#pragma once

#include "common/types/internal_types.h"
#include <cstdlib>

namespace infinity {
namespace {
String chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

inline void RandomString(char *ptr, SizeT len, u32 seed = 0) {
    srand(seed);
    for (SizeT i = 0; i < len; i++) {
        SizeT rand_i = random() % chars.size();
        ptr[i] = chars[rand_i];
    }
}
}