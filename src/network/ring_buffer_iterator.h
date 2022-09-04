//
// Created by JinHai on 2022/7/21.
//

#pragma once

#include "pg_message.h"
#include "common/utility/infinity_assert.h"

#include <array>
#include <boost/iterator/iterator_facade.hpp>

namespace infinity {

class RingBufferIterator : public boost::iterator_facade<RingBufferIterator, char, std::forward_iterator_tag, char &> {
public:
    explicit RingBufferIterator(std::array<char, PG_MSG_BUFFER_SIZE>& data, size_t position = 0)
            : data_(data), position_(position) {}

    RingBufferIterator& operator=(const RingBufferIterator& other) {
        NetworkAssert(&data_ == &other.data_, "The two iterators are from different arrays");
        position_ = other.position_;
        return *this;
    }

    // Below three member function are used by boost::iterator::iterator_facade
    [[nodiscard]] inline bool equal(RingBufferIterator const& other) const {
        return &data_ == &other.data_ && position_ == other.position_;
    }

    inline void increment() { position_ = (position_ + 1) % PG_MSG_BUFFER_SIZE; }

    [[nodiscard]] inline reference dereference() const { return data_[position_]; }

    std::array<char, PG_MSG_BUFFER_SIZE>& data_;
    size_t position_;
};

}
