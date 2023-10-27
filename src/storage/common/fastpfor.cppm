module;

import stl;

export module fastpfor;

namespace infinity {

// -------------------------------------------------------------------------------------
// the linker breaks when including the fastpfor library multiple times.
// thus, provide a wrapper for the functions we use from it.
// -------------------------------------------------------------------------------------
enum class FastPForCodec { FastPFor, SIMDBitPacking };

// -------------------------------------------------------------------------------------
export template <FastPForCodec Codec>
struct FastPForWrapper {
    FastPForWrapper();

    ~FastPForWrapper();

    u32 Compress(const u32 *src, u32 count, u32 *dest, SizeT &outsize) const;

    const u32 *Decompress(const u32 *src, u32 count, u32 *dest, SizeT &outsize) const;

    static void ApplyDelta(u32 *src, SizeT count);

    static void RevertDelta(u32 *src, SizeT count);

private:
    struct Impl;
    UniquePtr<Impl> impl_;
};

export using FastPFor = FastPForWrapper<FastPForCodec::FastPFor>;
export using SIMDBitPacking = FastPForWrapper<FastPForCodec::SIMDBitPacking>;

} // namespace infinity