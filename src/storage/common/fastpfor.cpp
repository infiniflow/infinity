// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;
#include <iostream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <codecfactory.h>
#include <compositecodec.h>
#include <deltautil.h>
#include <fastpfor.h>
#include <simdbinarypacking.h>
#include <simdfastpfor.h>
#include <simdnewpfor.h>
#include <streamvariablebyte.h>
#include <variablebyte.h>

#pragma GCC diagnostic pop

import stl;

module fastpfor;

namespace infinity {

template <>
struct FastPForWrapper<FastPForCodec::SIMDFastPFor>::Impl {
    // FastPFor128
    // FastPForLib::CompositeCodec<FastPForLib::SIMDFastPFor<4>, FastPForLib::VariableByte> codec;
    // FastPFor256
    FastPForLib::CompositeCodec<FastPForLib::SIMDFastPFor<8>, FastPForLib::VariableByte> codec;
};

template <>
struct FastPForWrapper<FastPForCodec::SIMDNewPFor>::Impl {
    FastPForLib::CompositeCodec<FastPForLib::SIMDNewPFor<4, FastPForLib::Simple16<false>>, FastPForLib::VariableByte> codec;
};

template <>
struct FastPForWrapper<FastPForCodec::StreamVByte>::Impl {
    FastPForLib::StreamVByte codec;
};

template <>
struct FastPForWrapper<FastPForCodec::SIMDBitPacking>::Impl {
    FastPForLib::CompositeCodec<FastPForLib::SIMDBinaryPacking, FastPForLib::VariableByte> codec;
};

template <FastPForCodec Codec>
FastPForWrapper<Codec>::FastPForWrapper() : impl_(new FastPForWrapper<Codec>::Impl) {}

template <FastPForCodec Codec>
FastPForWrapper<Codec>::~FastPForWrapper() = default;

template <FastPForCodec Codec>
u32 FastPForWrapper<Codec>::Compress(const u32 *src, u32 count, u32 *dest, SizeT &outsize) const {
    auto &codec = this->impl_->codec;
    codec.encodeArray(src, count, dest, outsize);
    return outsize;
}
// -------------------------------------------------------------------------------------
template <FastPForCodec Codec>
const u32 *FastPForWrapper<Codec>::Decompress(const u32 *src, u32 count, u32 *dest, SizeT &outsize) const {
    auto &codec = this->impl_->codec;
    return codec.decodeArray(src, count, dest, outsize);
}

template <FastPForCodec Codec>
void FastPForWrapper<Codec>::ApplyDelta(u32 *src, SizeT count) {
    using namespace FastPForLib;
    FastPForLib::Delta::deltaSIMD(src, count);
}

template <FastPForCodec Codec>
void FastPForWrapper<Codec>::RevertDelta(u32 *src, SizeT count) {
    using namespace FastPForLib;
    FastPForLib::Delta::inverseDeltaSIMD(src, count);
}

// template struct FastPForWrapper<FastPForCodec::FastPFor>;
template struct FastPForWrapper<FastPForCodec::SIMDBitPacking>;

} // namespace infinity