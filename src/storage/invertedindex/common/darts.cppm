module;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <darts.h>

#pragma GCC diagnostic pop

export module darts;

namespace infinity {
export template <typename A, typename B, typename T, typename C>
using DoubleArrayImpl = Darts::DoubleArrayImpl<A, B, T, C>;
export using DoubleArrayUnit = Darts::Details::DoubleArrayUnit;
} // namespace infinity
