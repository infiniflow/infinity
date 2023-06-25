#include "msb_radix_sort.h"
#include "bkd_writer.h"
#include "common/utility/infinity_assert.h"

#include <utility>
#include <algorithm>
#include <cassert>

namespace infinity {

MSBRadixSorter::MSBRadixSorter(
    BKDWriter* writer,
    PointWriter* point_writer,
    int dim,
    int max_length)
    :writer_(writer),
     point_writer_(point_writer),
     common_prefix_(std::vector<int>(std::min(24, max_length))),
     dim_(dim),
     max_length_(max_length) {
    pivot_ = std::make_shared<BytesRefBuilder>();
}

int MSBRadixSorter::ByteAt(int i, int k) {
    assert(k >= 0);
    if (k < writer_->bytes_per_dim_) {
        // dim bytes
        int block = i / point_writer_->values_per_block_;
        int index = i % point_writer_->values_per_block_;
        return point_writer_->blocks_[block][index * writer_->packed_bytes_length_ +
                                             dim_ * writer_->bytes_per_dim_ + k] & 0xff;
    } else {
        // doc id
        int s = 3 - (k - writer_->bytes_per_dim_);
        return (static_cast<int>(static_cast<unsigned int>(point_writer_->doc_IDs_[i]) >>
                                 (s * 8))) & 0xff;
    }
    if (k < writer_->bytes_per_dim_) {
        int32_t block = i / point_writer_->values_per_block_;
        int32_t index = i % point_writer_->values_per_block_;
        return point_writer_->blocks_[block][index * writer_->packed_bytes_length_ + dim_ * writer_->bytes_per_dim_ + k] & 0xff;
    } else {
        int32_t s = 3 - (k - writer_->bytes_per_dim_);
        return (static_cast<int>(static_cast<unsigned int>(point_writer_->doc_IDs_[i]) >> (s * 8))) & 0xff;
    }
}

void MSBRadixSorter::Swap(int i, int j) {
    int32_t doc_id = point_writer_->doc_IDs_[i];
    point_writer_->doc_IDs_[i] = point_writer_->doc_IDs_[j];
    point_writer_->doc_IDs_[j] = doc_id;

    if (!writer_->single_value_per_doc_) {
        if (writer_->long_ords_) {
            int64_t ord = point_writer_->ords_long_[j];
            point_writer_->ords_long_[i] = point_writer_->ords_long_[j];
            point_writer_->ords_long_[j] = ord;
        } else {
            int32_t ord = point_writer_->ords_[i];
            point_writer_->ords_[i] = point_writer_->ords_[j];
            point_writer_->ords_[j] = ord;
        }
    }

    int indexI = (i % point_writer_->values_per_block_) * writer_->packed_bytes_length_;
    int indexJ = (j % point_writer_->values_per_block_) * writer_->packed_bytes_length_;

    if (writer_->packed_bytes_length_ == 4) {
        auto *value1 = reinterpret_cast<uint32_t*>(point_writer_->blocks_[i / point_writer_->values_per_block_].data() + indexI);
        auto *value2 = reinterpret_cast<uint32_t*>(point_writer_->blocks_[j / point_writer_->values_per_block_].data() + indexJ);
        uint32_t tmp = *value1;
        *value1 = *value2;
        *value2 = tmp;
    }  else {
        auto& blockI = point_writer_->blocks_[i / point_writer_->values_per_block_];
        auto& blockJ = point_writer_->blocks_[j / point_writer_->values_per_block_];
        std::copy(blockI.begin() + indexI,
                  blockI.begin() + indexI + writer_->packed_bytes_length_,
                  writer_->scratch1_.begin());
        std::copy(blockJ.begin() + indexJ,
                  blockJ.begin() + indexJ + writer_->packed_bytes_length_,
                  blockI.begin() + indexI);
        std::copy(writer_->scratch1_.begin(),
                  writer_->scratch1_.begin() + writer_->packed_bytes_length_,
                  blockJ.begin() + indexJ);
    }
}

int MSBRadixSorter::Compare(int i, int j) {
    for (int o = k_; o < max_length_; ++o) {
        int b1 = ByteAt(i, o);
        int b2 = ByteAt(j, o);
        if (b1 != b2) {
            return b1 - b2;
        } else if (b1 == -1) {
            break;
        }
    }
    return 0;
}

void MSBRadixSorter::SetPivot(int i) {
    pivot_->SetLength(0);
    for (int o = k_; o < max_length_; ++o) {
        int b = ByteAt(i, o);
        if (b == -1) {
            break;
        }
        pivot_->Append(static_cast<char>(b));
    }
}

int MSBRadixSorter::ComparePivot(int j) {
    for (int o = 0; o < pivot_->Length(); ++o) {
        int b1 = pivot_->ByteAt(o) & 0xff;
        int b2 = ByteAt(j, k_ + o);
        if (b1 != b2) {
            return b1 - b2;
        }
    }
    if (k_ + pivot_->Length() == max_length_) {
        return 0;
    }
    return -1 - ByteAt(j, k_ + pivot_->Length());
}

void MSBRadixSorter::Sort(int from, int to) {
    StorageAssert(from < to, "'to' must be >= 'from'");
    Sort(from, to, 0, 0);
}

void MSBRadixSorter::Sort(int from, int to, int k, int l) {
    if (to - from <= LENGTH_THRESHOLD || l >= LEVEL_THRESHOLD) {
        k_ = k;
        QuickSort(from, to, k);
    } else {
        RadixSort(from, to, k, l);
    }
}

void MSBRadixSorter::QuickSort(int from, int to, int max_depth) {
    --max_depth;
    int mid = static_cast<int>(static_cast<unsigned int>((from + to)) >> 1);

    if (Compare(from, mid) > 0) {
        Swap(from, mid);
    }

    if (Compare(mid, to - 1) > 0) {
        Swap(mid, to - 1);
        if (Compare(from, mid) > 0) {
            Swap(from, mid);
        }
    }

    int left = from + 1;
    int right = to - 2;

    SetPivot(mid);
    for (;;) {
        while (ComparePivot(right) < 0) {
            --right;
        }

        while (left < right && ComparePivot(left) >= 0) {
            ++left;
        }

        if (left < right) {
            Swap(left, right);
            --right;
        } else {
            break;
        }
    }

    QuickSort(from, left + 1, max_depth);
    QuickSort(left + 1, to, max_depth);
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
            Swap(from + h1, from + h2);
        }
    }
}

}