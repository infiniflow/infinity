module;

import stl;

export module isnan;

namespace infinity {

class Catalog;

export void RegisterIsnanFunction(const UniquePtr<Catalog> &catalog_ptr);

}