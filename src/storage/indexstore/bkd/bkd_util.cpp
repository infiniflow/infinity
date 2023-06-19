#include "bkd_util.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

int BKDUtil::Mismatch(
    std::vector<uint8_t> &a,
    int aFromIndex,
    int aToIndex,
    std::vector<uint8_t> &b,
    int bFromIndex,
    int bToIndex) {
    StorageAssert(aFromIndex < aToIndex, "index bound error");
    StorageAssert(aFromIndex >= 0 && aToIndex <= a.size(), "index bound error");
    StorageAssert(bFromIndex < bToIndex, "index bound error");
    StorageAssert(bFromIndex >= 0 && bToIndex <= b.size(), "index bound error");
    int aLen = aToIndex - aFromIndex;
    int bLen = bToIndex - bFromIndex;
    int len = std::min(aLen, bLen);
    for (int i = 0; i < len; i++) {
        if (a[i + aFromIndex] != b[i + bFromIndex]) {
            return i;
        }
    }
    return aLen == bLen ? -1 : len;
}

int BKDUtil::CompareUnsigned(
    const uint8_t *a,
    int aFromIndex,
    int aToIndex,
    const uint8_t *b,
    int bFromIndex,
    int bToIndex) {
    int aLen = aToIndex - aFromIndex;
    int bLen = bToIndex - bFromIndex;
    int len = std::min(aLen, bLen);
    for (int i = 0; i < len; i++) {
        int aByte = a[i + aFromIndex] & 0xFF;
        int bByte = b[i + bFromIndex] & 0xFF;
        int diff = aByte - bByte;
        if (diff != 0) {
            return diff;
        }
    }
    // One is a prefix of the other, or, they are equal:
    return aLen - bLen;
}

void BKDUtil::Subtract(
    int bytesPerDim,
    int dim,
    std::vector<uint8_t> &a,
    std::vector<uint8_t> &b,
    std::vector<uint8_t> &result) {
    int start = dim * bytesPerDim;
    int end = start + bytesPerDim;
    int borrow = 0;
    for (int i = end - 1; i >= start; i--) {
        int diff = (a[i] & 0xff) - (b[i] & 0xff) - borrow;
        if (diff < 0) {
            diff += 256;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i - start] = static_cast<char>(diff);
    }
    StorageAssert(borrow == 0, "illegal argument");
}

void BKDUtil::WriteDocIds(
    std::vector<int32_t> &docids,
    int32_t start,
    int32_t count,
    FileWriter *out) {
    bool sorted = true;
    for (int32_t i = 1; i < count; ++i) {
        if (docids[start + i - 1] > docids[start + i]) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        out->WriteByte((uint8_t) 0);
        int32_t previous = 0;
        for (int32_t i = 0; i < count; ++i) {
            int32_t doc = docids[start + i];
            out->WriteVInt(doc - previous);
            previous = doc;
        }
    } else {
        int64_t max = 0;
        for (int32_t i = 0; i < count; ++i) {
            max |= (static_cast<uint64_t>(docids[start + i]));
        }

        if (max <= 0xffffff) {
            out->WriteByte((uint8_t) 24);
            for (int32_t i = 0; i < count; ++i) {
                out->WriteShort((short) (docids[start + i] >> 8));
                out->WriteByte((uint8_t) (docids[start + i]));
            }
        } else {
            out->WriteByte((uint8_t) 32);
            for (int32_t i = 0; i < count; ++i) {
                out->WriteInt(docids[start + i]);
            }
        }
    }
}

void BKDUtil::WriteDocIdsBitmap(
    std::vector<int32_t> &docids,
    int32_t start,
    int32_t count,
    FileWriter *out) {
    roaring::Roaring r;
    for (int32_t i = 0; i < count; ++i) {
        int32_t doc = docids[start + i];
        r.add(doc);
    }
    r.runOptimize();
    auto bitmap_size = r.getSizeInBytes(false);
    out->WriteVInt(bitmap_size);
    char *bitmap = new char[bitmap_size];
    r.write(bitmap, false);
    out->Write((const char*)bitmap, bitmap_size);
    delete[] bitmap;
}

}