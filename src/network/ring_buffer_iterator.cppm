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
import default_values;
import global_resource_usage;

export module ring_buffer_iterator;

namespace infinity {

export class RingBufferIterator {
public:
    explicit RingBufferIterator(Array<char, PG_MSG_BUFFER_SIZE> &data, SizeT position = 0) : data_(data), position_(position) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("RingBufferIterator");
#endif
    }

    RingBufferIterator(const RingBufferIterator &other) : data_(other.data_), position_(other.position_) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("RingBufferIterator");
#endif
    }

    ~RingBufferIterator() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("RingBufferIterator");
#endif
    }

    RingBufferIterator &operator=(const RingBufferIterator &other) {
        //        Assert<NetworkException>(&data_ == &other.data_, "The two iterators are from different arrays");
        position_ = other.position_;
        return *this;
    }

    // Below three member function are used by boost::iterator::iterator_facade
    [[nodiscard]] inline bool equal(RingBufferIterator const &other) const { return &data_ == &other.data_ && position_ == other.position_; }

    inline void increment() { position_ = (position_ + 1) % PG_MSG_BUFFER_SIZE; }

    inline void increment(SizeT n) { position_ = (position_ + n) % PG_MSG_BUFFER_SIZE; }

    [[nodiscard]] inline char dereference() const { return data_[position_]; }

    inline char *position_addr() const { return &data_[position_]; }

    static inline i64 Distance(const RingBufferIterator &begin_iter, const RingBufferIterator &end_iter) {
        return (i64)end_iter.position_ - (i64)begin_iter.position_;
    }

    static inline RingBufferIterator Find(const RingBufferIterator &begin_iter, const RingBufferIterator &end_iter, char finding) {
        RingBufferIterator iter = begin_iter;
        while (!iter.equal(end_iter)) {
            if (iter.dereference() == finding) {
                return iter;
            } else {
                iter.increment();
            }
        }
        return iter;
    }

    static inline void Copy(const RingBufferIterator &begin_iter, const RingBufferIterator &end_iter, String &output) {
        SizeT size = Distance(begin_iter, end_iter);
        output.reserve(size);
        RingBufferIterator iter = begin_iter;
        while (!iter.equal(end_iter)) {
            output.push_back(iter.dereference());
            iter.increment();
        }
    }

    static inline void CopyN(const RingBufferIterator &begin_iter, SizeT len, char *output) {
        RingBufferIterator iter = begin_iter;
        for (SizeT idx = 0; idx < len; ++idx) {
            output[idx] = iter.dereference();
            iter.increment();
        }
    }

    static inline void CopyN(const RingBufferIterator &begin_iter, SizeT len, String &result) {
        RingBufferIterator iter = begin_iter;
        for (SizeT idx = 0; idx < len; ++idx) {
            result.push_back(iter.dereference());
            iter.increment();
        }
    }

    static inline void CopyN(const char *src, SizeT len, RingBufferIterator &result_iter) {
        for (SizeT str_idx = 0, array_idx = result_iter.position_; str_idx < len; ++str_idx, array_idx = (array_idx + 1) % PG_MSG_BUFFER_SIZE) {
            result_iter.data_[array_idx] = src[str_idx];
        }
    }

    Array<char, PG_MSG_BUFFER_SIZE> &data_;
    u32 position_;
};

} // namespace infinity