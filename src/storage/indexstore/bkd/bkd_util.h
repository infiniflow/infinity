#pragma once

#include "storage/io/file_writer.h"

#include <cstdint>
#include <vector>
#include <roaring/roaring.hh>

namespace infinity {

class BKDUtil {
public:
    static int Mismatch(
        std::vector<uint8_t> &a,
        int aFromIndex,
        int aToIndex,
        std::vector<uint8_t> &b,
        int bFromIndex,
        int bToIndex);

    static int CompareUnsigned(
        const uint8_t *a,
        int aFromIndex,
        int aToIndex,
        const uint8_t *b,
        int bFromIndex,
        int bToIndex);

    static void Subtract(
        int bytesPerDim,
        int dim,
        std::vector<uint8_t> &a,
        std::vector<uint8_t> &b,
        std::vector<uint8_t> &result);

    static void WriteDocIds(
        std::vector<int32_t> &docids,
        int32_t start,
        int32_t count,
        FileWriter *out);


    static void WriteDocIdsBitmap(
        std::vector<int32_t> &docids,
        int32_t start,
        int32_t count,
        FileWriter *out);


};
}