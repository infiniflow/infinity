//
// Created by jinhai on 23-11-5.
//

#pragma once

#include "status.h"

namespace infinity {

#define STATUS_CHECK(func) \
    do {                   \
        Status s = func;   \
        if (!s.ok()) {     \
            return s;      \
        }                  \
    } while (false)
}
