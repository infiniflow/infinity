#pragma once
#include "common/types/internal_types.h"
#include <memory>
#include <type_traits>

namespace infinity {


// -------------------------------------------------------------------------------------
// the linker breaks when including the fastpfor library multiple times.
// thus, provide a wrapper for the functions we use from it.
// -------------------------------------------------------------------------------------
enum class FastPForCodec { FastPFor, SIMDBitPacking };
// -------------------------------------------------------------------------------------
template <FastPForCodec Codec>
struct FastPForWrapper {
    FastPForWrapper();

    ~FastPForWrapper();

    u32 Compress(const u32* src, u32 count, u32* dest, size_t& outsize);

    const u32* Decompress(const u32* src, u32 count, u32* dest, size_t& outsize);

    static void ApplyDelta(u32* src, size_t count);

    static void RevertDelta(u32* src, size_t count);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

using FastPFor = FastPForWrapper<FastPForCodec::FastPFor>;
using SIMDBitPacking = FastPForWrapper<FastPForCodec::SIMDBitPacking>;

}