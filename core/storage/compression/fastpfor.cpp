#include "fastpfor.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <headers/blockpacking.h>
#include <headers/codecfactory.h>
#include <headers/deltautil.h>
#include <headers/fastpfor.h>

#pragma GCC diagnostic pop

namespace infinity {

template <>
struct FastPForWrapper<FastPForCodec::FastPFor>::Impl {
    FastPForLib::CompositeCodec<FastPForLib::SIMDFastPFor<8>, FastPForLib::VariableByte> codec;
};

template <>
struct FastPForWrapper<FastPForCodec::SIMDBitPacking>::Impl {
    // FastPForLib::CompositeCodec<FastPForLib::FastBinaryPacking<32>, FastPForLib::VariableByte> codec;
    FastPForLib::CompositeCodec<FastPForLib::SIMDBinaryPacking, FastPForLib::VariableByte> codec;
};

template <FastPForCodec Codec>
FastPForWrapper<Codec>::FastPForWrapper() : impl_(new FastPForWrapper<Codec>::Impl) {}

template <FastPForCodec Codec>
FastPForWrapper<Codec>::~FastPForWrapper() = default;

template <FastPForCodec Codec>
u32 FastPForWrapper<Codec>::Compress(const u32 *src, u32 count, u32 *dest, size_t &outsize) const {
    auto &codec = this->impl_->codec;
    codec.encodeArray(src, count, dest, outsize);
    return outsize;
}
// -------------------------------------------------------------------------------------
template <FastPForCodec Codec>
const u32 *FastPForWrapper<Codec>::Decompress(const u32 *src, u32 count, u32 *dest, size_t &outsize) const {
    auto &codec = this->impl_->codec;
    return codec.decodeArray(src, count, dest, outsize);
}

template <FastPForCodec Codec>
void FastPForWrapper<Codec>::ApplyDelta(u32 *src, size_t count) {
    using namespace FastPForLib;
    FastPForLib::Delta::deltaSIMD(src, count);
}

template <FastPForCodec Codec>
void FastPForWrapper<Codec>::RevertDelta(u32 *src, size_t count) {
    using namespace FastPForLib;
    FastPForLib::Delta::inverseDeltaSIMD(src, count);
}

template struct FastPForWrapper<FastPForCodec::FastPFor>;
template struct FastPForWrapper<FastPForCodec::SIMDBitPacking>;

} // namespace infinity
