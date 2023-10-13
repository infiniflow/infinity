//
// Created by jinhai on 23-3-20.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterSubstringFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
