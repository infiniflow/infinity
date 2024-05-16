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

import stl;

export module fastpfor;

namespace infinity {

export enum class FastPForCodec { SIMDFastPFor, SIMDNewPFor, StreamVByte, SIMDBitPacking };

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

export using SIMDFastPFor = FastPForWrapper<FastPForCodec::SIMDFastPFor>;
export using SIMDNewPFor = FastPForWrapper<FastPForCodec::SIMDNewPFor>;
export using StreamVByte = FastPForWrapper<FastPForCodec::StreamVByte>;
export using SIMDBitPacking = FastPForWrapper<FastPForCodec::SIMDBitPacking>;

} // namespace infinity