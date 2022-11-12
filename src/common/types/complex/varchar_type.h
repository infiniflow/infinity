//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/varlen_type.h"

namespace infinity {

struct VarcharType {
    ptr_t ptr{nullptr};   // pointer to the varchar value buffer.
    i16 length{0};  // 65535 will be the limitation.
    char_t prefix[6] {}; // prefix of the varchar
};

struct Char1Type {
    char_t value[1]{};
};

struct Char2Type {
    char_t value[2]{};
};

struct Char4Type {
    char_t value[4]{};
};

struct Char8Type {
    char_t value[8]{};
};

struct Char16Type {
    i8     length{0};
    char_t value[15]{};
};

struct Char32Type {
    i8     length{0};
    char_t value[31]{};
};

struct Char64Type {
    i8     length{0};
    char_t value[63]{};
};

}

