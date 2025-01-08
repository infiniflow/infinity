module;

export module trunc;

import stl;

namespace infinity {

class Catalog;
export void RegisterTruncFunction(const UniquePtr<Catalog> &catalog_ptr);

}