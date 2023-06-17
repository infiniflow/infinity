#pragma once

#include <vector>


namespace infinity {
//MSB Radix sort from lucene
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

    // store one histogram per recursion level
    std::vector<std::vector<int>> histograms_ =
                                   std::vector<std::vector<int>>(LEVEL_THRESHOLD);
    std::vector<int> end_offsets_ = std::vector<int>(HISTOGRAM_SIZE);
    std::vector<int> common_prefix_;

    const int max_length_;

protected:
    explicit MSBRadixSorter(int maxLength);

    virtual int ByteAt(int i, int k) = 0;
    virtual void swap(int i, int j) = 0;
public:
    void Sort(int from, int to);

private:
    void Sort(int from, int to, int k, int l);


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