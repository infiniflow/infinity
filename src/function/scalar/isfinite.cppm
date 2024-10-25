module;

import stl;

export module isfinite;

namespace infinity {

class Catalog;

export void RegisterIsfiniteFunction(const UniquePtr<Catalog> &catalog_ptr);

}