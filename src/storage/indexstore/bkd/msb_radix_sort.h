#pragma once

#include "bytesref.h"

#include <vector>
#include <memory>

namespace infinity {
class BKDWriter;
class PointWriter;

//MSB Radix sort from lucene 7.5
class MSBRadixSorter {
    // after that many levels of recursion we fall back to quicksort anyway
    // this is used as a protection against the fact that radix sort performs
    // worse when there are long common prefixes (probably because of cache
    // locality)
private:
    static constexpr int LEVEL_THRESHOLD = 8;
    // size of histograms: 256 + 1 to indicate that the string is finished
    static constexpr int HISTOGRAM_SIZE = 257;
    // buckets below this size will be sorted with quicksort
    static constexpr int LENGTH_THRESHOLD = 100;

    static constexpr int BINARY_SORT_THRESHOLD = 20;

    BKDWriter * writer_;

    PointWriter * point_writer_;

    // store one histogram per recursion level
    std::vector<std::vector<int>> histograms_ =
                                   std::vector<std::vector<int>>(LEVEL_THRESHOLD);
    std::vector<int> end_offsets_ = std::vector<int>(HISTOGRAM_SIZE);

    std::vector<int> common_prefix_;

    const int max_length_;

    const int dim_;

    int k_ = 0;

    std::shared_ptr<BytesRefBuilder> pivot_;
public:
    explicit MSBRadixSorter(
        BKDWriter* writer,
        PointWriter* point_writer,
        int dim,
        int max_length);

public:
    void Sort(int from, int to);

private:
    int ByteAt(int i, int k);

    void Swap(int i, int j);

    void BinarySort(int from, int to);

    void BinarySort(int from, int to, int i);

    void HeapSort(int from, int to);

    void Heapify(int from, int to);

    void SiftDown(int i, int from, int to);

    int HeapParent(int from, int i);

    int HeapChild(int from, int i);

    void Sort(int from, int to, int k, int l);

    int Compare(int i, int j);

    void SetPivot(int i);

    int ComparePivot(int j);

    void QuickSort(int from, int to, int max_depth);

    void RadixSort(int from, int to, int k, int l);

    static bool AssertHistogram(int commonPrefixLength, std::vector<int> &histogram);

    int GetBucket(int i, int k);

    int ComputeCommonPrefixLengthAndBuildHistogram(int from, int to, int k,
            std::vector<int> &histogram);

    void BuildHistogram(int from, int to, int k, std::vector<int> &histogram);

    static void SumHistogram(std::vector<int> &histogram,
                             std::vector<int> &endOffsets);

    void Reorder(int from, int to, std::vector<int> &start,
                 std::vector<int> &end, int k);
};

}