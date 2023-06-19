#pragma once

#include <vector>
#include <cstdint>

namespace infinity {

class BitSet {
private:
    std::vector<int64_t> bits_;
    const int64_t num_bits_;
    const int num_words_;
public:
    explicit BitSet(int64_t num_bits)
        : bits_(std::vector<int64_t>(static_cast<int>((num_bits - 1) >> 6) + 1)),
          num_bits_(num_bits),
          num_words_(bits_.size()) {}

    void Set(int64_t index) {
        int word_num = static_cast<int>(index >> 6);
        int64_t bitmask = 1LL << index;
        bits_[word_num] |= bitmask;
    }

    bool Get(int64_t index) {
        int i = static_cast<int>(index >> 6);
        int64_t bitmask = 1LL << index;
        return (bits_[i] & bitmask) != 0;
    }

    void Clear(int64_t index) {
        int wordNum = static_cast<int>(index >> 6);
        int64_t bitmask = 1LL << index;
        bits_[wordNum] &= ~bitmask;
    }
};

}
