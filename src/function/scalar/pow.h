//
// Created by JinHai on 2022/9/16.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterPowFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}