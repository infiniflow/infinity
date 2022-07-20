//
// Created by JinHai on 2022/7/20.
//

#pragma once

#include <string>

namespace infinity {

// trim path string "/absolute/path/src/module/abc.h" into "module/abc.h"
std::string TrimPath(const std::string& path);

}
