
#pragma once

#include <fst/fst-decl.h>

#include <cstdint>

namespace fst {

template <typename Label>
class StringLeftWeight;

template <typename Label>
class StringRefWeight;

template <typename W, typename L>
struct ILabelArc;

template <typename Arc>
class ImmutableFst;

} // namespace fst

namespace infinity {
using byte_type = uint8_t;
using byte_weight = fst::StringLeftWeight<byte_type>;
using byte_arc = fst::ArcTpl<byte_weight>;
using vector_byte_fst = fst::VectorFst<byte_arc, fst::VectorState<byte_arc, std::allocator<byte_arc>>>;

using byte_ref_weight = fst::StringRefWeight<byte_type>;
using byte_ref_arc = fst::ILabelArc<byte_ref_weight, int32_t>;
using immutable_byte_fst = fst::ImmutableFst<byte_ref_arc>;

template <typename Key, typename Weight, typename Stats>
class FSTBuilder;

} // namespace infinity