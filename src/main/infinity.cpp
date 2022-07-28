//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"

namespace infinity {

Infinity::Infinity()
    : scheduler_(std::make_shared<NaiveScheduler>()) {}

}
