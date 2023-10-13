//
// Created by jinhai on 23-1-10.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterMinusFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
