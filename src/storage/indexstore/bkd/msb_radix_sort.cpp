#include "msb_radix_sort.h"
#include "common/utility/infinity_assert.h"

#include <utility>
#include <algorithm>
#include <cassert>

namespace infinity {

MSBRadixSorter::MSBRadixSorter(int max_length)
    : common_prefix_(std::vector<int>(std::min(24, max_length))), max_length_(max_length) {
}

void MSBRadixSorter::Sort(int from, int to) {
    StorageAssert(to < from, "'to' must be >= 'from'");
    Sort(from, to, 0, 0);
}

void MSBRadixSorter::Sort(int from, int to, int k, int l) {
    if (to - from <= LENGTH_THRESHOLD || l >= LEVEL_THRESHOLD) {
    } else {
        RadixSort(from, to, k, l);
    }
}


void MSBRadixSorter::RadixSort(int from, int to, int k, int l) {
    std::vector<int> histogram = histograms_[l];
    if (histogram.empty()) {
        histogram = histograms_[l] = std::vector<int>(HISTOGRAM_SIZE);
    } else {
        std::fill(histogram.begin(), histogram.end(), 0);
    }

    int common_prefix_length =
        ComputeCommonPrefixLengthAndBuildHistogram(from, to, k, histogram);
    if (common_prefix_length > 0) {
        // if there are no more chars to compare or if all entries fell into the
        // first bucket (which means strings are shorter than k) then we are done
        // otherwise recurse
        if (k + common_prefix_length < max_length_ && histogram[0] < to - from) {
            RadixSort(from, to, k + common_prefix_length, l);
        }
        return;
    }
    assert((AssertHistogram(common_prefix_length, histogram)));

    SumHistogram(histogram, end_offsets_);
    Reorder(from, to, histogram, end_offsets_, k);

    if (k + 1 < max_length_) {
        // recurse on all but the first bucket since all keys are equals in this
        // bucket (we already compared all bytes)
        for (int prev = histogram[0], i = 1; i < HISTOGRAM_SIZE; ++i) {
            int h = histogram[i];
            int bucketLen = h - prev;
            if (bucketLen > 1) {
                Sort(from + prev, from + h, k + 1, l + 1);
            }
            prev = h;
        }
    }
}

bool MSBRadixSorter::AssertHistogram(int common_prefix_length,
                                     std::vector<int> &histogram) {
    int numberOfUniqueBytes = 0;
    for (auto freq: histogram) {
        if (freq > 0) {
            numberOfUniqueBytes++;
        }
    }
    if (numberOfUniqueBytes == 1) {
        assert(common_prefix_length >= 1);
    } else {
        assert(common_prefix_length == 0);
    }
    return true;
}

int MSBRadixSorter::GetBucket(int i, int k) {
    return ByteAt(i, k) + 1;
}

int MSBRadixSorter::ComputeCommonPrefixLengthAndBuildHistogram(
    int from, int to, int k, std::vector<int> &histogram) {
    int common_prefix_length = std::min(int(common_prefix_.size()), max_length_ - k);
    for (int j = 0; j < common_prefix_length; ++j) {
        int b = ByteAt(from, k + j);
        common_prefix_[j] = b;
        if (b == -1) {
            common_prefix_length = j + 1;
            break;
        }
    }

    int i;
    for (i = from + 1; i < to; ++i) {
        for (int j = 0; j < common_prefix_length; ++j) {
            int b = ByteAt(i, k + j);
            if (b != common_prefix_[j]) {
                common_prefix_length = j;
                if (common_prefix_length == 0) {// we have no common prefix
                    histogram[common_prefix_[0] + 1] = i - from;
                    histogram[b + 1] = 1;
                    goto outerBreak;
                }
                break;
            }
        }
    }
outerBreak:

    if (i < to) {
        // the loop got broken because there is no common prefix
        assert(common_prefix_length == 0);
        BuildHistogram(i + 1, to, k, histogram);
    } else {
        assert(common_prefix_length > 0);
        histogram[common_prefix_[0] + 1] = to - from;
    }

    return common_prefix_length;
}

void MSBRadixSorter::BuildHistogram(int from, int to, int k,
                                    std::vector<int> &histogram) {
    for (int i = from; i < to; ++i) {
        histogram[GetBucket(i, k)]++;
    }
}

void MSBRadixSorter::SumHistogram(std::vector<int> &histogram,
                                  std::vector<int> &endOffsets) {
    int accum = 0;
    for (int i = 0; i < HISTOGRAM_SIZE; ++i) {
        int count = histogram[i];
        histogram[i] = accum;
        accum += count;
        endOffsets[i] = accum;
    }
}

void MSBRadixSorter::Reorder(int from, int to, std::vector<int> &start,
                             std::vector<int> &end, int k) {
    // reorder in place, like the dutch flag problem
    for (int i = 0; i < HISTOGRAM_SIZE; ++i) {
        const int limit = end[i];
        for (int h1 = start[i]; h1 < limit; h1 = start[i]) {
            const int b = GetBucket(from + h1, k);
            const int h2 = start[b]++;
            swap(from + h1, from + h2);
        }
    }
}

}