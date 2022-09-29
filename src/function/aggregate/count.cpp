//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include <sstream>


namespace infinity {

std::string
Count::ToString() {
    std::stringstream ss;
    ss << "count()" << std::endl;
    return ss.str();
}
}
