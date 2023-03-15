//
// Created by JinHai on 2022/7/20.
//

#pragma once

#include "common/types/internal_types.h"

#include <string>
#include <algorithm>
#include <cctype>
#include <locale>

namespace infinity {

// trim path string "/absolute/path/src/module/abc.h" into "module/abc.h"
String
TrimPath(const String& path);

inline void
StringToLower(String& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](const auto c) {
        return std::tolower(c);
    });
}

// trim from start (in place)
inline void
ltrim(String &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && (ch != '\0');
    }));
}

// trim from end (in place)
inline void
rtrim(String &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && (ch != '\0');
    }).base(), s.end());
}

// trim from both ends (in place)
inline void
trim(String &s) {
    ltrim(s);
    rtrim(s);
}

}
