//
// Created by jinhai on 23-10-22.
//

module;

#include "stdlib.h"

import stl;

module random;

namespace infinity {
namespace {
String available_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

inline String RandomString(SizeT len, u32 seed) {
    srand(seed);
    String ret(len, '\0');
    for (SizeT i = 0; i < len; i++) {
        SizeT rand_i = random() % available_chars.size();
        ret[i] = available_chars[rand_i];
    }
    return ret;
}
} // namespace infinity
