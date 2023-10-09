//
// Created by JinHai on 2022/7/20.
//

#include "str.h"

namespace infinity {

std::string TrimPath(const std::string &path) {
    const auto pos = path.find("/src/");
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1);
}

} // namespace infinity
