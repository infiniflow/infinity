//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function/aggregate_function.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterAvgFunction(const UniquePtr<Catalog> &catalog_ptr);

}