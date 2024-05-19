/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <memory>
#include <sstream>

#include "decompress_sse4.h"
#include "pack.h"
#include "s9compressor.h"
#include "unaligned_unpack.h"
#include "unpack.h"

namespace indexlib {
struct NewPForDeltaCompressorHeader {
    uint32_t isLastBlock       : 1;
    uint32_t exceptionIntRange : 11;
    uint32_t frameBits         : 5;
    uint32_t firstExceptionPos : 8; // 0:no exception, 1~128: first execption pos, 250 ~255: optimize flag
    uint32_t dataNum           : 7;
    inline NewPForDeltaCompressorHeader(uint32_t x = 0)
    {
        assert(sizeof(*this) == sizeof(uint32_t));
        *(uint32_t*)(this) = x;
    }
    inline uint32_t ToUint32() const { return *(const uint32_t*)(this); }
};

class NewPForDeltaCompressor
{
public:
    const static uint32_t EXCEPTION_THRESHOLD_RATE = 10; // exception rate 10%
    const static uint32_t BLOCK_SIZE = 128;              // atomic block size for compression
    const static uint32_t HEADER_INT_SIZE = 1;

    // special exception pos value for optimization
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE = 255;     // value store in exceptionIntRange
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE = 254;      // value store in 4 byte after header
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE = 253; // min value store in exceptionIntRange
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE =
        252; // min value store in 4 byte after header
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE = 251; // min value store in exceptionIntRange
    const static uint32_t SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE =
        250; // min value store in 4 byte after header

    static int64_t BASIC_MASK[];
    static uint32_t FRAME_BIT_MAP[];

public:
    NewPForDeltaCompressor();
    ~NewPForDeltaCompressor();

public:
    /**
     * Compress int32 array to /dest/
     * @param dest byte array to store compressed bytes
     * @param destLen length of /dest/ array
     * @param src source uint32 array
     * @param srcLen length of /src/ array
     * @return compressed length in int32 size
     */
    size_t CompressInt32(uint32_t* dest, size_t destLen, const uint32_t* src, size_t srcLen,
                                            bool enableBlockOpt = false) const;

    /**
     * Compress int16 array to /dest/
     * @param dest byte array to store compressed bytes
     * @param destLen length of /dest/ array
     * @param src source uint16 array
     * @param srcLen length of /src/ array
     * @return compressed length in int32 size
     */
    size_t CompressInt16(uint32_t* dest, size_t destLen, const uint16_t* src, size_t srcLen,
                                            bool enableBlockOpt = false) const;

    /**
     * Compress int8 array to /dest/
     * @param dest byte array to store compressed bytes
     * @param destLen length of /dest/ array
     * @param src source uint8 array
     * @param srcLen length of /src/ array
     * @return compressed length in int32 size
     */
    size_t CompressInt8(uint32_t* dest, size_t destLen, const uint8_t* src, size_t srcLen,
                                           bool enableBlockOpt = false) const;

    /**
     * Decompress compressed array to uint32 array
     * @param dest int32 array to store decompressed bytes
     * @param destLen length of /dest/ array
     * @param src compressed uint32 array
     * @param srcLen length of /src/ array
     * @return number of decompressed integers
     */
    size_t DecompressInt32(uint32_t* dest, size_t destLen, const uint32_t* src, size_t srcLen) const;

    /**
     * Decompress compressed array to uint16 array
     * @param dest int16 array to store decompressed bytes
     * @param destLen length of /dest/ array
     * @param src compressed uint32 array
     * @param srcLen length of /src/ array
     * @return number of decompressed integers
     */
    size_t DecompressInt16(uint16_t* dest, size_t destLen, const uint32_t* src, size_t srcLen) const;

    /**
     * Decompress compressed array to uint8 array
     * @param dest int8 array to store decompressed bytes
     * @param destLen length of /dest/ array
     * @param src compressed uint32 array
     * @param srcLen length of /src/ array
     * @return number of decompressed integers
     */
    size_t DecompressInt8(uint8_t* dest, size_t destLen, const uint32_t* src, size_t srcLen) const;

    /**
     * Get compressed length by parsing header
     * @return the length of compressed block in int32 size, including header bytes
     */
    inline size_t GetCompressedLength(uint32_t header) const;

    /**
     * Check if this compressed block is the last block
     * @return true if it's last block, otherwise false
     */
    inline bool IsLastBlock(uint32_t header) const;

public:
    template <typename T>
    static size_t Decompress(T* dest, size_t destLen, const uint32_t* src, size_t srcLen);

    template <typename T>
    static size_t Compress(uint32_t* dest, size_t destLen, const T* src, size_t srcLen,
                                              bool enableBlockOpt = false);

    template <typename T>
    static size_t DecompressBlock(T* dest, size_t destLen, uint32_t*& src, size_t& srcLen, bool& lastFlag);

    template <typename T>
    static bool TryDecompressSpecialBlock(T* dest, uint32_t dataNum, uint32_t firstExceptionPos,
                                          uint32_t exceptionIntRange, uint32_t*& src, size_t& srcLen);

protected:
    template <typename T>
    static size_t CompressBlock(uint32_t* dest, size_t destLen, const T* src, size_t srcLen,
                                                   uint32_t frameBits, uint32_t numExceptions, bool isLastBlock);

protected:
    template <typename T>
    static uint32_t SelectCompressBits(const T* block, size_t blkLen, bool enableBlockOpt, uint32_t& numExceptions);

    template <typename T>
    static size_t TransformToFrame(uint32_t* dest, size_t destLen, uint32_t frameBits,
                                                      size_t firstExceptionPos, const T* code, size_t codeLen,
                                                      uint32_t* exception, uint32_t* exceptionOffset,
                                                      size_t numExceptions, bool lastFlag);
    template <typename T>
    static size_t TransformToSpecialFrame(uint32_t* dest, size_t destLen, const T* code, size_t codeLen,
                                          size_t numExceptions, bool lastFlag);

    template <typename T>
    static void Pack(uint32_t* dest, const T* src, size_t srcLen, uint32_t frameBits);

protected:
    static uint32_t MakeHeader(uint32_t frameBits, size_t firstExceptionPos, size_t codeLen, uint32_t numException,
                               uint32_t exceptionIntRange, bool isLastBlock);

    static void PackException(uint32_t* dest, size_t exceptionOffsetIntSize, uint32_t* compressedExceptionDatum,
                              size_t len);

    template <typename T>
    static size_t S9Encode(uint32_t* dest, size_t destLen, T* src, size_t srcLen);

    template <typename T>
    static void S9Decode(T* dest, uint32_t* src, size_t srcLen, size_t firstExceptionPos, uint32_t frameBits);

    inline static uint32_t HighBitIdx(uint32_t value);

    inline static void FastMemZero(uint32_t* dest, size_t destLen);

    template <typename T>
    inline static void FastMemSet(T* dest, size_t destLen, T value);

    inline static bool IsUniqValueStoreInline(uint32_t value);

};

////////////////////////////////////////////////////////////////////////////////////
//

template <typename T>
size_t NewPForDeltaCompressor::Compress(uint32_t* dest, size_t destLen, const T* src, size_t srcLen,
                                                           bool enableBlockOpt)
{
    uint32_t* destPtr = dest;
    size_t destLeftLen = destLen;
    size_t totalBlk = (srcLen + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (size_t off = 0, blk = 0; blk < totalBlk; off += BLOCK_SIZE, ++blk) {
        size_t left = srcLen - off;
        size_t len = (left <= BLOCK_SIZE) ? left : BLOCK_SIZE;
        uint32_t bestExceptions;
        uint32_t bestFrameBits = SelectCompressBits(src + off, len, enableBlockOpt, bestExceptions);
        auto blkCompSize =
            CompressBlock(destPtr, destLeftLen, src + off, len, bestFrameBits, bestExceptions, (blk == totalBlk - 1));
        destPtr += blkCompSize;
        destLeftLen -= blkCompSize;
    }
    return destPtr - dest;
}

template <typename T>
inline uint32_t NewPForDeltaCompressor::SelectCompressBits(const T* block, size_t blkLen, bool enableBlockOpt,
                                                           uint32_t& numExceptions)
{
    uint32_t numBitArray[33] = {0};
    T firstValue {};
    bool isAllSame = true;
    bool isIncrease = true;
    bool isDecrease = true;
    uint32_t maxBits = 0;
    for (size_t i = 0; i < blkLen; ++i) {
        if (i == 0) {
            firstValue = block[i];
        }
        if (isAllSame) {
            isAllSame = (block[i] == firstValue);
        }
        if (isIncrease) {
            isIncrease = (block[i] == (firstValue + (T)i));
        }
        if (isDecrease) {
            isDecrease = (block[i] == (firstValue - (T)i));
        }
        uint32_t highBit = HighBitIdx(block[i]);
        maxBits = highBit > maxBits ? highBit : maxBits;
        ++numBitArray[highBit];
    }

    if (maxBits == 0) {
        numExceptions = 0;
        return 0;
    }

    if (enableBlockOpt && isAllSame) {
        numExceptions = IsUniqValueStoreInline(firstValue) ? SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE
                                                           : SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE;
        return 0;
    }
    if (enableBlockOpt && isIncrease) {
        numExceptions = IsUniqValueStoreInline(firstValue) ? SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE
                                                           : SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE;
        return 0;
    }
    if (enableBlockOpt && isDecrease) {
        T baseValue = (blkLen > 0) ? block[blkLen - 1] : firstValue;
        numExceptions = IsUniqValueStoreInline(baseValue) ? SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE
                                                          : SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE;
        return 0;
    }

    for (int j = 1; j < 33; j++) {
        numBitArray[j] += numBitArray[j - 1];
    }

    uint32_t maxExceptionNum = (blkLen * EXCEPTION_THRESHOLD_RATE) / 100;
    numExceptions = 0;
    uint32_t bestFrameBits = 32;
    uint32_t bestTotalBytes = sizeof(uint32_t) * blkLen;

    for (int32_t i = FRAME_BIT_MAP[maxBits]; i >= 0; i--) {
        if ((int32_t)FRAME_BIT_MAP[i] != i) {
            continue;
        }
        uint32_t frameBits = i;
        uint32_t exceptionNum = blkLen - numBitArray[i];
        if (exceptionNum > maxExceptionNum || maxBits > frameBits + 28) {
            break;
        }

        uint32_t exceptionBits = maxBits - frameBits;
        uint32_t bytesPerException = 0;
        if (exceptionBits <= 7) {
            bytesPerException = 2;
        } else if (exceptionBits <= 9) {
            bytesPerException = 3;
        } else if (exceptionBits <= 14) {
            bytesPerException = 4;
        } else {
            bytesPerException = 8;
        }

        uint32_t bytesForFrame = (blkLen * frameBits + 31) / 32 * 4;                 // round to 4x
        uint32_t bytesForException = (exceptionNum * bytesPerException + 3) / 4 * 4; // round to 4x
        uint32_t totalBytes = bytesForException + bytesForFrame;
        if (totalBytes < bestTotalBytes) {
            bestTotalBytes = totalBytes;
            bestFrameBits = frameBits;
            numExceptions = exceptionNum;
        }
    }
    return bestFrameBits;
}

template <typename T>
size_t NewPForDeltaCompressor::CompressBlock(uint32_t* dest, size_t destLen, const T* src,
                                                                size_t srcLen, uint32_t frameBits,
                                                                uint32_t numExceptions, bool isLastBlock)
{
    uint32_t pre = 0;
    T max = (T)(1 << frameBits);
    T code[BLOCK_SIZE] {};
    T miss[BLOCK_SIZE];
    uint32_t exception[BLOCK_SIZE];

    if (numExceptions == 0) {
        // firstExceptionPos = 0 represent no exception
        return TransformToFrame(dest, destLen, frameBits, 0, src, srcLen, NULL, NULL, 0, isLastBlock);
    }

    if (numExceptions == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE) {
        return TransformToSpecialFrame(dest, destLen, src, srcLen, numExceptions, isLastBlock);
    }

    // Loop 1: find exception
    size_t j = 0;
    miss[0] = 0; // avoid warn miss[0] uninitialized
    for (size_t i = 0; i < srcLen; i++) {
        T val = src[i];
        code[i] = val;
        miss[j] = (T)i;
        if (max <= val) {
            j++;
        }
    }

    assert(j == numExceptions);
    // Loop 2: create offset and upper-bit value list for patch .
    uint32_t exceptionOffset[BLOCK_SIZE];

    T cur = miss[0];
    exception[0] = (uint32_t)(code[cur] >> frameBits);
    code[cur] = code[cur] & BASIC_MASK[frameBits];
    pre = cur;

    for (size_t i = 1; i < j; i++) {
        cur = miss[i];
        exception[i] = code[cur] >> frameBits;
        code[cur] = code[cur] & BASIC_MASK[frameBits];
        exceptionOffset[i - 1] = cur - pre - 1;
        pre = cur;
    }

    size_t firstExceptionPos = miss[0] + 1;
    return TransformToFrame(dest, destLen, frameBits, firstExceptionPos, code, srcLen, exception, exceptionOffset,
                            numExceptions, isLastBlock);
}

template <typename T>
inline size_t NewPForDeltaCompressor::TransformToSpecialFrame(uint32_t* dest, size_t destLen, const T* code,
                                                              size_t codeLen, size_t numExceptions, bool lastFlag)
{
    assert(numExceptions == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE ||
           numExceptions == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE ||
           numExceptions == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE ||
           numExceptions == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE ||
           numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE ||
           numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE);

    assert(codeLen > 0);
    bool storeAlone = (numExceptions == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE ||
                       numExceptions == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE ||
                       numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE);
    size_t exceptionOffsetIntSize = HEADER_INT_SIZE;
    if (storeAlone) {
        ++exceptionOffsetIntSize;
    }
    FastMemZero(dest, exceptionOffsetIntSize);

    T value = code[0];
    if (numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE ||
        numExceptions == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE) {
        value = code[codeLen - 1];
    }
    *dest++ = MakeHeader(0, numExceptions, codeLen, numExceptions, storeAlone ? 0 : value, lastFlag);
    if (storeAlone) {
        *dest++ = (uint32_t)value;
    }
    return exceptionOffsetIntSize;
}

template <typename T>
inline size_t
NewPForDeltaCompressor::TransformToFrame(uint32_t* dest, size_t destLen, uint32_t frameBits, size_t firstExceptionPos,
                                         const T* code, size_t codeLen, uint32_t* exception, uint32_t* exceptionOffset,
                                         size_t numExceptions, bool isLastBlock)
{
    size_t exceptionOffsetIntSize = HEADER_INT_SIZE + (codeLen * frameBits + 31) / 32;
    if (firstExceptionPos == 0) {
        assert(destLen >= exceptionOffsetIntSize);
        FastMemZero(dest, exceptionOffsetIntSize);

        *dest++ = MakeHeader(frameBits, firstExceptionPos, codeLen, 0, 0, isLastBlock);
        Pack(dest, code, codeLen, frameBits);

        return exceptionOffsetIntSize;
    }

    // make exception region
    uint32_t exceptionOffsetNum = numExceptions - 1; // exceptionOffset.length;

    uint32_t exceptionDatum[BLOCK_SIZE];
    assert(BLOCK_SIZE >= exceptionOffsetNum + numExceptions);
    for (size_t i = 0; i < numExceptions; i++) {
        exceptionDatum[2 * i] = exception[i];
    }
    for (uint32_t i = 0; i < exceptionOffsetNum; i++) {
        exceptionDatum[2 * i + 1] = exceptionOffset[i];
    }

    // Encode exception value
    auto exceptionRange = S9Encode(dest + exceptionOffsetIntSize, destLen - exceptionOffsetIntSize,
                                             exceptionDatum, exceptionOffsetNum + numExceptions);

    size_t intDataSize = exceptionOffsetIntSize + exceptionRange;
    assert(destLen >= intDataSize);

    FastMemZero(dest, exceptionOffsetIntSize);

    // 1: make header
    // size of header == sizeof(uint32_t)
    *dest++ = MakeHeader(frameBits, firstExceptionPos, codeLen, numExceptions, exceptionRange, isLastBlock);

    // 2: make encoded value
    Pack(dest, code, codeLen, frameBits);

    return intDataSize;
}

template <typename T>
inline void NewPForDeltaCompressor::Pack(uint32_t* dest, const T* src, size_t srcLen, uint32_t frameBits)
{
    typedef void (*pack_function)(uint32_t*, const T*, uint32_t n);
    static pack_function pack_func[33] = {
        pack_0<T>,  pack_1<T>,  pack_2<T>,  pack_3<T>,  pack_4<T>,  pack_5<T>,  pack_6<T>,  pack_7<T>,  pack_8<T>,
        pack_9<T>,  pack_10<T>, pack_11<T>, pack_12<T>, pack_13<T>, pack_14<T>, pack_15<T>, pack_16<T>, pack_17<T>,
        pack_18<T>, pack_19<T>, pack_20<T>, pack_21<T>, pack_22<T>, pack_23<T>, pack_24<T>, pack_25<T>, pack_26<T>,
        pack_27<T>, pack_28<T>, pack_29<T>, pack_30<T>, pack_31<T>, pack_32<T>};

    (*pack_func[frameBits])(dest, src, srcLen);
}

template <typename T>
size_t NewPForDeltaCompressor::Decompress(T* dest, size_t destLen, const uint32_t* src, size_t srcLen)
{
    uint32_t* srcPtr = (uint32_t*)src;
    size_t encodeLen = srcLen;
    size_t totalDecodeSize = 0;
    bool isLastBlock = false;
    while (!isLastBlock) {
        totalDecodeSize +=
            DecompressBlock(dest + totalDecodeSize, destLen - totalDecodeSize, srcPtr, encodeLen, isLastBlock);
    }
    return totalDecodeSize;
}

template <typename T>
inline bool NewPForDeltaCompressor::TryDecompressSpecialBlock(T* dest, uint32_t dataNum, uint32_t firstExceptionPos,
                                                              uint32_t exceptionIntRange, uint32_t*& src,
                                                              size_t& srcLen)
{
    uint32_t value = exceptionIntRange;
    switch (firstExceptionPos) {
    case SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE: {
        FastMemSet(dest, dataNum, (T)value);
        src += HEADER_INT_SIZE;
        srcLen -= HEADER_INT_SIZE;
        break;
    }
    case SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE: {
        value = *(src + HEADER_INT_SIZE);
        FastMemSet(dest, dataNum, (T)value);
        src += (HEADER_INT_SIZE + 1);
        srcLen -= (HEADER_INT_SIZE + 1);
        break;
    }
    case SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE: {
        for (uint32_t i = 0; i < dataNum; i++) {
            dest[i] = (T)value + (T)i;
        }
        src += HEADER_INT_SIZE;
        srcLen -= HEADER_INT_SIZE;
        break;
    }
    case SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE: {
        value = *(src + HEADER_INT_SIZE);
        for (uint32_t i = 0; i < dataNum; i++) {
            dest[i] = (T)value + (T)i;
        }
        src += (HEADER_INT_SIZE + 1);
        srcLen -= (HEADER_INT_SIZE + 1);
        break;
    }
    case SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE: {
        for (uint32_t i = 0; i < dataNum; i++) {
            dest[dataNum - i - 1] = (T)value + (T)i;
        }
        src += HEADER_INT_SIZE;
        srcLen -= HEADER_INT_SIZE;
        break;
    }
    case SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE: {
        value = *(src + HEADER_INT_SIZE);
        for (uint32_t i = 0; i < dataNum; i++) {
            dest[dataNum - i - 1] = (T)value + (T)i;
        }
        src += (HEADER_INT_SIZE + 1);
        srcLen -= (HEADER_INT_SIZE + 1);
        break;
    }
    default:
        return false;
    }
    return true;
}

template <typename T>
inline size_t NewPForDeltaCompressor::DecompressBlock(T* dest, size_t destLen, uint32_t*& src, size_t& srcLen,
                                                      bool& isLastBlock)
{
    // Step 1. decode header value
    // Header component is encoded as follow:
    //
    // 7bit : dataNum - 1
    // 8bit : first exceptionPos
    // 5bit : numFramebit
    // 11bit : exception byte range
    // 1bit : has next frame or not
    //
    NewPForDeltaCompressorHeader header(*src);
    uint32_t dataNum = header.dataNum + 1;
    // miss[0] + 1 or 0
    uint32_t firstExceptionPos = header.firstExceptionPos;
    // 0 <= numFramebit <= 3
    uint32_t frameBits = header.frameBits == 31 ? 32 : header.frameBits;
    uint32_t exceptionIntRange = header.exceptionIntRange;
    isLastBlock = header.isLastBlock;
    if (TryDecompressSpecialBlock(dest, dataNum, firstExceptionPos, exceptionIntRange, src, srcLen)) {
        return dataNum;
    }

    typedef void (*unpack_function)(T*, const uint32_t*, uint32_t n);
    static unpack_function unpack_func[33] = {
        unpack_0<T>,  unpack_1<T>,  unpack_2<T>,  unpack_3<T>,  unpack_4<T>,  unpack_5<T>,  unpack_6<T>,
        unpack_7<T>,  unpack_8<T>,  unpack_9<T>,  unpack_10<T>, unpack_11<T>, unpack_12<T>, unpack_13<T>,
        unpack_14<T>, unpack_15<T>, unpack_16<T>, unpack_17<T>, unpack_18<T>, unpack_19<T>, unpack_20<T>,
        unpack_21<T>, unpack_22<T>, unpack_23<T>, unpack_24<T>, unpack_25<T>, unpack_26<T>, unpack_27<T>,
        unpack_28<T>, unpack_29<T>, unpack_30<T>, unpack_31<T>, unpack_32<T>};

    /// Step 2. decode normal data
    (*unpack_func[frameBits])(dest, src + 1, (uint32_t)dataNum);

    size_t intOffsetForExceptionRange = HEADER_INT_SIZE + (dataNum * frameBits + 31) / 32;

    /// Step 3. decode exception data
    if (firstExceptionPos != 0) {
        S9Decode(dest, src + intOffsetForExceptionRange, exceptionIntRange, firstExceptionPos - 1, frameBits);
    }

    src += (intOffsetForExceptionRange + exceptionIntRange);
    srcLen -= (intOffsetForExceptionRange + exceptionIntRange);
    return dataNum;
}

template <>
inline size_t NewPForDeltaCompressor::DecompressBlock<uint32_t>(uint32_t* dest, size_t destLen, uint32_t*& src,
                                                                size_t& srcLen, bool& isLastBlock)
{
    // Step 1. decode header value
    // Header component is encoded as follow:
    //
    // 7bit : dataNum - 1
    // 8bit : first exceptionPos
    // 5bit : numFramebit
    // 11bit : exception byte range
    // 1bit : has next frame or not
    //
    NewPForDeltaCompressorHeader header(*src);
    uint32_t dataNum = header.dataNum + 1;
    // miss[0] + 1 or 0
    uint32_t firstExceptionPos = header.firstExceptionPos;
    // 0 <= numFramebit <= 3
    uint32_t frameBits = header.frameBits == 31 ? 32 : header.frameBits;
    uint32_t exceptionIntRange = header.exceptionIntRange;
    isLastBlock = header.isLastBlock;
    if (TryDecompressSpecialBlock(dest, dataNum, firstExceptionPos, exceptionIntRange, src, srcLen)) {
        return dataNum;
    }

    typedef void (*unpack_sse_function)(uint32_t*, const uint32_t*, uint32_t n);
    static unpack_sse_function unpack_sse_func[33] = {
        decompress_sse4_c0,  decompress_sse4_c1,  decompress_sse4_c2,  decompress_sse4_c3,  decompress_sse4_c4,
        decompress_sse4_c5,  unpack_6<uint32_t>,  decompress_sse4_c7,  decompress_sse4_c8,  decompress_sse4_c9,
        decompress_sse4_c10, decompress_sse4_c11, decompress_sse4_c12, decompress_sse4_c13, decompress_sse4_c14,
        unpack_15<uint32_t>, decompress_sse4_c16, decompress_sse4_c17, decompress_sse4_c18, decompress_sse4_c19,
        decompress_sse4_c20, decompress_sse4_c21, decompress_sse4_c22, decompress_sse4_c23, decompress_sse4_c24,
        decompress_sse4_c25, decompress_sse4_c26, decompress_sse4_c27, decompress_sse4_c28, decompress_sse4_c29,
        decompress_sse4_c30, decompress_sse4_c31, decompress_sse4_c32};

    /// Step 2. decode normal data
    (*unpack_sse_func[frameBits])(dest, src + 1, (uint32_t)dataNum);

    size_t intOffsetForExceptionRange = HEADER_INT_SIZE + (dataNum * frameBits + 31) / 32;

    /// Step 3. decode exception data
    if (firstExceptionPos != 0) {
        S9Decode(dest, src + intOffsetForExceptionRange, exceptionIntRange, firstExceptionPos - 1, frameBits);
    }

    src += (intOffsetForExceptionRange + exceptionIntRange);
    srcLen -= (intOffsetForExceptionRange + exceptionIntRange);
    return dataNum;
}

//////////////////////////////////////////////////////////////
// inline functions

inline uint32_t NewPForDeltaCompressor::MakeHeader(uint32_t frameBits, size_t firstExceptionPos, size_t codeLen,
                                                   uint32_t numExceptions, uint32_t exceptionIntRange, bool isLastBlock)
{
    NewPForDeltaCompressorHeader header;
    uint32_t lastOrNot = isLastBlock ? 1 : 0;
    if (frameBits >= 32) {
        frameBits = 31;
    }
    header.dataNum = (uint32_t)codeLen - 1;
    header.firstExceptionPos = firstExceptionPos;
    assert(firstExceptionPos <= 128 || firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE ||
           firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE ||
           firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE ||
           firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE ||
           firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE ||
           firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE);
    header.frameBits = frameBits;
    header.exceptionIntRange = exceptionIntRange;
    header.isLastBlock = lastOrNot;
    return header.ToUint32();
}

inline void NewPForDeltaCompressor::PackException(uint32_t* dest, size_t exceptionOffsetIntSize,
                                                  uint32_t* compressedExceptionDatum, size_t len)
{
    size_t offset = exceptionOffsetIntSize;
    for (size_t i = 0; i < len; i++) {
        dest[offset++] = compressedExceptionDatum[i];
    }
}

#if defined(__aarch64__)
#include <arm_neon.h>
inline uint32_t NewPForDeltaCompressor::HighBitIdx(uint32_t value)
{
    int r = 0;

    if (value != 0)
    {
        uint32x2_t v = vdup_n_u32(value);
        r = 31 - __builtin_clz(vget_lane_u32(v, 1));

        if (r == -1)
            r = 31 - __builtin_clz(vget_lane_u32(v, 0));
    }

    return static_cast<uint32_t>(r + 1);
}
#else
inline uint32_t NewPForDeltaCompressor::HighBitIdx(uint32_t value)
{
    int r = 0;

    __asm__("bsrl %1,%0\n\t"
            "cmovzl %2,%0"
            : "=&r"(r)
            : "rm"(value), "rm"(-1));
    return (uint32_t)(r + 1);
}
#endif

template <typename T>
inline size_t NewPForDeltaCompressor::S9Encode(uint32_t* dest, size_t destLen, T* src, size_t srcLen)
{
    return S9Compressor::Encode(dest, destLen, src, srcLen);
}

template <typename T>
inline void NewPForDeltaCompressor::FastMemSet(T* dest, size_t destLen, T value)
{
    uint32_t maxBlocks = destLen >> 4;
    uint32_t rest = destLen % 16;

    for (uint32_t blk = 0; blk < maxBlocks; blk++, dest += 16) {
        dest[0] = value;
        dest[1] = value;
        dest[2] = value;
        dest[3] = value;
        dest[4] = value;
        dest[5] = value;
        dest[6] = value;
        dest[7] = value;
        dest[8] = value;
        dest[9] = value;
        dest[10] = value;
        dest[11] = value;
        dest[12] = value;
        dest[13] = value;
        dest[14] = value;
        dest[15] = value;
    }

    for (uint32_t i = 0; i < rest; ++i) {
        dest[i] = value;
    }
}

inline void NewPForDeltaCompressor::FastMemZero(uint32_t* dest, size_t destLen)
{
    FastMemSet(dest, destLen, (uint32_t)0);
}

template <typename T>
inline void NewPForDeltaCompressor::S9Decode(T* dest, uint32_t* src, size_t srcLen, size_t firstExceptionPos,
                                             uint32_t frameBits)
{
    size_t currentPos = firstExceptionPos;
    uint32_t head = 0;
    for (size_t i = 0; i < srcLen; i++) {
        uint32_t val = src[i];
        uint32_t header = (val >> 28) + head;

        switch (header) {
        case 0: // code num : 28, bitwidth : 1
        {
            dest[currentPos] |= ((val << 4) >> 31) << frameBits;
            currentPos += ((val << 5) >> 31) + 1;
            dest[currentPos] |= ((val << 6) >> 31) << frameBits;
            currentPos += ((val << 7) >> 31) + 1;
            dest[currentPos] |= ((val << 8) >> 31) << frameBits;
            currentPos += ((val << 9) >> 31) + 1;
            dest[currentPos] |= ((val << 10) >> 31) << frameBits;
            currentPos += ((val << 11) >> 31) + 1;
            dest[currentPos] |= ((val << 12) >> 31) << frameBits;
            currentPos += ((val << 13) >> 31) + 1; // 10
            dest[currentPos] |= ((val << 14) >> 31) << frameBits;
            currentPos += ((val << 15) >> 31) + 1;
            dest[currentPos] |= ((val << 16) >> 31) << frameBits;
            currentPos += ((val << 17) >> 31) + 1;
            dest[currentPos] |= ((val << 18) >> 31) << frameBits;
            currentPos += ((val << 19) >> 31) + 1;
            dest[currentPos] |= ((val << 20) >> 31) << frameBits;
            currentPos += ((val << 21) >> 31) + 1;
            dest[currentPos] |= ((val << 22) >> 31) << frameBits;
            currentPos += ((val << 23) >> 31) + 1; // 20
            dest[currentPos] |= ((val << 24) >> 31) << frameBits;
            currentPos += ((val << 25) >> 31) + 1;
            dest[currentPos] |= ((val << 26) >> 31) << frameBits;
            currentPos += ((val << 27) >> 31) + 1;
            dest[currentPos] |= ((val << 28) >> 31) << frameBits;
            currentPos += ((val << 29) >> 31) + 1;
            dest[currentPos] |= ((val << 30) >> 31) << frameBits;
            currentPos += ((val << 31) >> 31) + 1;
            head = 0;
            break;
        }
        case 1: // code num : 14, bitwidth : 2
        {
            dest[currentPos] |= ((val << 4) >> 30) << frameBits;
            currentPos += ((val << 6) >> 30) + 1;
            dest[currentPos] |= ((val << 8) >> 30) << frameBits;
            currentPos += ((val << 10) >> 30) + 1;
            dest[currentPos] |= ((val << 12) >> 30) << frameBits;
            currentPos += ((val << 14) >> 30) + 1;
            dest[currentPos] |= ((val << 16) >> 30) << frameBits;
            currentPos += ((val << 18) >> 30) + 1;
            dest[currentPos] |= ((val << 20) >> 30) << frameBits;
            currentPos += ((val << 22) >> 30) + 1; // 10
            dest[currentPos] |= ((val << 24) >> 30) << frameBits;
            currentPos += ((val << 26) >> 30) + 1;
            dest[currentPos] |= ((val << 28) >> 30) << frameBits;
            currentPos += ((val << 30) >> 30) + 1;
            head = 0;
            break;
        }
        case 2: // code num : 9, bitwidth : 3
        {
            dest[currentPos] |= ((val << 5) >> 29) << frameBits;
            currentPos += ((val << 8) >> 29) + 1;
            dest[currentPos] |= ((val << 11) >> 29) << frameBits;
            currentPos += ((val << 14) >> 29) + 1;
            dest[currentPos] |= ((val << 17) >> 29) << frameBits;
            currentPos += ((val << 20) >> 29) + 1;
            dest[currentPos] |= ((val << 23) >> 29) << frameBits;
            currentPos += ((val << 26) >> 29) + 1;
            dest[currentPos] |= ((val << 29) >> 29) << frameBits;
            head = 16;
            break;
        }
        case 3: // code num : 7, bitwidth : 4
        {
            dest[currentPos] |= ((val << 4) >> 28) << frameBits;
            currentPos += ((val << 8) >> 28) + 1;
            dest[currentPos] |= ((val << 12) >> 28) << frameBits;
            currentPos += ((val << 16) >> 28) + 1;
            dest[currentPos] |= ((val << 20) >> 28) << frameBits;
            currentPos += ((val << 24) >> 28) + 1;
            dest[currentPos] |= ((val << 28) >> 28) << frameBits;
            head = 16;
            break;
        }
        case 4: // code num : 5, bitwidth : 5
        {
            dest[currentPos] |= ((val << 7) >> 27) << frameBits;
            currentPos += ((val << 12) >> 27) + 1;
            dest[currentPos] |= ((val << 17) >> 27) << frameBits;
            currentPos += ((val << 22) >> 27) + 1;
            dest[currentPos] |= ((val << 27) >> 27) << frameBits;
            head = 16;
            break;
        }
        case 5: // code num : 4, bitwidth : 7
        {
            dest[currentPos] |= ((val << 4) >> 25) << frameBits;
            currentPos += ((val << 11) >> 25) + 1;
            dest[currentPos] |= ((val << 18) >> 25) << frameBits;
            currentPos += ((val << 25) >> 25) + 1;
            head = 0;
            break;
        }
        case 6: // code num : 3, bitwidth : 9
        {
            dest[currentPos] |= ((val << 5) >> 23) << frameBits;
            currentPos += ((val << 14) >> 23) + 1;
            dest[currentPos] |= ((val << 23) >> 23) << frameBits;
            head = 16;
            break;
        }
        case 7: // code num : 2, bitwidth : 14
        {
            dest[currentPos] |= ((val << 4) >> 18) << frameBits;
            currentPos += ((val << 18) >> 18) + 1;
            head = 0;
            break;
        }
        case 8: // code num : 1, bitwidth : 28
        {
            dest[currentPos] |= ((val << 4) >> 4) << frameBits;
            head = 16;
            break;
        }

        case 16: // code num : 28, bitwidth : 1
        {
            currentPos += ((val << 4) >> 31) + 1;
            dest[currentPos] |= ((val << 5) >> 31) << frameBits;
            currentPos += ((val << 6) >> 31) + 1;
            dest[currentPos] |= ((val << 7) >> 31) << frameBits;
            currentPos += ((val << 8) >> 31) + 1;
            dest[currentPos] |= ((val << 9) >> 31) << frameBits;
            currentPos += ((val << 10) >> 31) + 1;
            dest[currentPos] |= ((val << 11) >> 31) << frameBits;
            currentPos += ((val << 12) >> 31) + 1;
            dest[currentPos] |= ((val << 13) >> 31) << frameBits; // 10
            currentPos += ((val << 14) >> 31) + 1;
            dest[currentPos] |= ((val << 15) >> 31) << frameBits;
            currentPos += ((val << 16) >> 31) + 1;
            dest[currentPos] |= ((val << 17) >> 31) << frameBits;
            currentPos += ((val << 18) >> 31) + 1;
            dest[currentPos] |= ((val << 19) >> 31) << frameBits;
            currentPos += ((val << 20) >> 31) + 1;
            dest[currentPos] |= ((val << 21) >> 31) << frameBits;
            currentPos += ((val << 22) >> 31) + 1;
            dest[currentPos] |= ((val << 23) >> 31) << frameBits; // 20
            currentPos += ((val << 24) >> 31) + 1;
            dest[currentPos] |= ((val << 25) >> 31) << frameBits;
            currentPos += ((val << 26) >> 31) + 1;
            dest[currentPos] |= ((val << 27) >> 31) << frameBits;
            currentPos += ((val << 28) >> 31) + 1;
            dest[currentPos] |= ((val << 29) >> 31) << frameBits;
            currentPos += ((val << 30) >> 31) + 1;
            dest[currentPos] |= ((val << 31) >> 31) << frameBits;
            head = 16;
            break;
        }
        case 17: // code num : 14, bitwidth : 2
        {
            currentPos += ((val << 4) >> 30) + 1;
            dest[currentPos] |= ((val << 6) >> 30) << frameBits;
            currentPos += ((val << 8) >> 30) + 1;
            dest[currentPos] |= ((val << 10) >> 30) << frameBits;
            currentPos += ((val << 12) >> 30) + 1;
            dest[currentPos] |= ((val << 14) >> 30) << frameBits;
            currentPos += ((val << 16) >> 30) + 1;
            dest[currentPos] |= ((val << 18) >> 30) << frameBits;
            currentPos += ((val << 20) >> 30) + 1;
            dest[currentPos] |= ((val << 22) >> 30) << frameBits; // 10
            currentPos += ((val << 24) >> 30) + 1;
            dest[currentPos] |= ((val << 26) >> 30) << frameBits;
            currentPos += ((val << 28) >> 30) + 1;
            dest[currentPos] |= ((val << 30) >> 30) << frameBits;
            head = 16;
            break;
        }
        case 18: // code num : 9, bitwidth : 3
        {
            currentPos += ((val << 5) >> 29) + 1;
            dest[currentPos] |= ((val << 8) >> 29) << frameBits;
            currentPos += ((val << 11) >> 29) + 1;
            dest[currentPos] |= ((val << 14) >> 29) << frameBits;
            currentPos += ((val << 17) >> 29) + 1;
            dest[currentPos] |= ((val << 20) >> 29) << frameBits;
            currentPos += ((val << 23) >> 29) + 1;
            dest[currentPos] |= ((val << 26) >> 29) << frameBits;
            currentPos += ((val << 29) >> 29) + 1;
            head = 0;
            break;
        }
        case 19: // code num : 7, bitwidth : 4
        {
            currentPos += ((val << 4) >> 28) + 1;
            dest[currentPos] |= ((val << 8) >> 28) << frameBits;
            currentPos += ((val << 12) >> 28) + 1;
            dest[currentPos] |= ((val << 16) >> 28) << frameBits;
            currentPos += ((val << 20) >> 28) + 1;
            dest[currentPos] |= ((val << 24) >> 28) << frameBits;
            currentPos += ((val << 28) >> 28) + 1;
            head = 0;
            break;
        }
        case 20: // code num : 5, bitwidth : 5
        {
            currentPos += ((val << 7) >> 27) + 1;
            dest[currentPos] |= ((val << 12) >> 27) << frameBits;
            currentPos += ((val << 17) >> 27) + 1;
            dest[currentPos] |= ((val << 22) >> 27) << frameBits;
            currentPos += ((val << 27) >> 27) + 1;
            head = 0;
            break;
        }
        case 21: // code num : 4, bitwidth : 7
        {
            currentPos += ((val << 4) >> 25) + 1;
            dest[currentPos] |= ((val << 11) >> 25) << frameBits;
            currentPos += ((val << 18) >> 25) + 1;
            dest[currentPos] |= ((val << 25) >> 25) << frameBits;
            head = 16;
            break;
        }
        case 22: // code num : 3, bitwidth : 9
        {
            currentPos += ((val << 5) >> 23) + 1;
            dest[currentPos] |= ((val << 14) >> 23) << frameBits;
            currentPos += ((val << 23) >> 23) + 1;
            head = 0;
            break;
        }
        case 23: // code num : 2, bitwidth : 14
        {
            currentPos += ((val << 4) >> 18) + 1;
            dest[currentPos] |= ((val << 18) >> 18) << frameBits;
            head = 16;
            break;
        }
        case 24: // code num : 1, bitwidth : 28
        {
            currentPos += ((val << 4) >> 4) + 1;
            head = 0;
            break;
        }
        default:
            assert(false);
        } // end switch
    }     // end for
}

typedef std::shared_ptr<NewPForDeltaCompressor> NewPForDeltaCompressorPtr;

///////////////////////////////////////////////////////////
inline size_t NewPForDeltaCompressor::GetCompressedLength(uint32_t header) const
{
    // Header component is encoded as follow:
    //
    // 7bit : dataNum - 1
    // 8bit : first exceptionPos
    // 5bit : numFramebit
    // 11bit : exception byte range
    // 1bit : has next frame or not
    //
    NewPForDeltaCompressorHeader compressHeader(header);
    uint32_t dataNum = compressHeader.dataNum + 1;
    uint32_t frameBits = compressHeader.frameBits == 31 ? 32 : compressHeader.frameBits;
    uint32_t exceptionIntRange = compressHeader.exceptionIntRange;
    if (compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_INLINE ||
        compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_INLINE ||
        compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_INLINE) {
        return HEADER_INT_SIZE;
    }
    if (compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_SAME_VALUE_ALONE ||
        compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_INCREASE_VALUE_ALONE ||
        compressHeader.firstExceptionPos == SPECIAL_EXCEPTION_POS_FOR_DECREASE_VALUE_ALONE) {
        return HEADER_INT_SIZE + 1;
    }
    return (HEADER_INT_SIZE + (dataNum * frameBits + 31) / 32 + exceptionIntRange);
}

inline bool NewPForDeltaCompressor::IsLastBlock(uint32_t header) const
{
    NewPForDeltaCompressorHeader compressHeader(header);
    return compressHeader.isLastBlock;
}

inline bool NewPForDeltaCompressor::IsUniqValueStoreInline(uint32_t value)
{
    return value <= (uint32_t)2047; // 2 ^ 11 - 1
}
}
