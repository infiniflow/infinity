//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterSumFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}