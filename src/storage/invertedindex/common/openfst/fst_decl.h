
#pragma once

#include <fst/fst-decl.h>

#include <cstdint>

namespace fst {

template <typename Label>
class FSTStringLeftWeight;

template <typename Label>
class StringRefWeight;

template <typename W, typename L>
struct ILabelArc;

template <typename Arc>
class ImmutableFst;

} // namespace fst

namespace infinity {
using byte_type = uint8_t;
using ByteWeight = fst::FSTStringLeftWeight<byte_type>;
using ByteArc = fst::ArcTpl<ByteWeight>;
using VectorByteFST = fst::VectorFst<ByteArc, fst::VectorState<ByteArc, std::allocator<ByteArc>>>;

using ByteRefWeight = fst::StringRefWeight<byte_type>;
using ByteRefArc = fst::ILabelArc<ByteRefWeight, int32_t>;
using ImmutableByteFST = fst::ImmutableFst<ByteRefArc>;

template <typename Key, typename Weight, typename Stats>
class FSTBuilder;

} // namespace infinity