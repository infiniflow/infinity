module;

import stl;

export module hnsw_common;

namespace infinity {
export SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

} // namespace infinity