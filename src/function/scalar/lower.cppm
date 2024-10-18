module;

import stl;

export module lower;

namespace infinity {

class Catalog;

export void RegisterLowerFunction(const UniquePtr<Catalog> &catalog_ptr);

}