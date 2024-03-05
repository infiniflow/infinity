module;

#include <cassert>

export module ring;

import stl;

namespace infinity {

export template <typename T>
class Ring {
private:
    std::condition_variable cv_full_;
    std::condition_variable cv_empty_;
    std::mutex mutex_;
    Vector<T> ring_buf_; // An element is allowed to insert into the ring if its offset in inside [off_ground_, off_ground_+2^cap_mask_)
    u64 cap_shift_;
    u64 cap_mask_;
    u64 off_ground_;  // min element offset inside the ring
    u64 off_ceiling_; // 1 + max element offset inside the ring
    u64 off_filled_;  // elements with offset inside range [off_ground_, off_filled_) is a consecutive slice of ring, which should be
                      // ready to next stage
    u64 seq_get_;     // sequence number of get operation
public:
    Ring(u64 cap_shift) : cap_shift_(cap_shift), cap_mask_((1 << cap_shift) - 1), off_ground_(0), off_ceiling_(0), off_filled_(0), seq_get_(0) {
        ring_buf_.resize(1 << cap_shift);
    }

    void Put(u64 off, T elem) {
        T zero{};
        std::unique_lock<std::mutex> lock(mutex_);
        assert(off > off_filled_);
        if (off >= off_ground_ + (1 << cap_mask_))
            cv_full_.wait(lock, [this, off] { return off < off_ground_ + (1 << cap_shift_); });
        ring_buf_[off & cap_mask_] = elem;
        if (off_ceiling_ < off + 1) {
            off_ceiling_ = off + 1;
        }
        if (off == off_filled_) {
            off_filled_++;
            while (off_filled_ < off_ceiling_ && ring_buf_[off_filled_ & cap_mask_] != zero) {
                off_filled_++;
            }
        }
        cv_empty_.notify_one();
    }

    u64 Size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return off_ceiling_ - off_ground_;
    }

    u64 Get(T &elem) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (off_ground_ == off_filled_) {
            cv_empty_.wait(lock, [this] { return off_ground_ < off_filled_; });
        }
        elem = ring_buf_[off_ground_ & cap_mask_];
        off_ground_++;
        u64 seq = seq_get_++;
        cv_full_.notify_one();
        return seq;
    }

    u64 GetBatch(Vector<T> &batch) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (off_ground_ == off_filled_) {
            cv_empty_.wait(lock, [this] { return off_ground_ < off_filled_; });
        }
        batch.clear();
        while (off_ground_ < off_filled_) {
            T elem = ring_buf_[off_ground_ & cap_mask_];
            batch.push_back(elem);
            off_ground_++;
        }
        u64 seq = seq_get_++;
        cv_full_.notify_one();
        return seq;
    }

    void Iterate(std::function<void(T &)> func) {
        std::unique_lock<std::mutex> lock(mutex_);
        for (u64 off = off_ground_; off < off_filled_; off++) {
            func(ring_buf_[off & cap_mask_]);
        }
        off_ground_ = off_filled_;
    }
};
} // namespace infinity