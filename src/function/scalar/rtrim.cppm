module;

import stl;

export module rtrim;

namespace infinity {

class Catalog;

export void RegisterRtrimFunction(const UniquePtr<Catalog> &catalog_ptr);

}