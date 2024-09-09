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

export module roaring_bitmap;
import stl;
import croaring;
import infinity_exception;
import serialize;

namespace infinity {

template <bool init_all_true>
struct OptionalAllTrueFlag;

template <>
struct OptionalAllTrueFlag<true> {
    bool value = true;
};

template <>
struct OptionalAllTrueFlag<false> {};

template <bool init_all_true>
struct RoaringBitmap {
    static SharedPtr<RoaringBitmap> MakeSharedAllTrue(u32 count)
        requires init_all_true
    {
        return MakeShared<RoaringBitmap>(count);
    }

    static SharedPtr<RoaringBitmap> MakeSharedAllFalse(u32 count)
        requires(!init_all_true)
    {
        return MakeShared<RoaringBitmap>(count);
    }

    RoaringBitmap() = default;

    explicit RoaringBitmap(const u32 count) : count_(count) {}

    [[nodiscard]] inline auto count() const { return count_; }

    [[nodiscard]] inline bool IsAllTrue() const {
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return true;
            }
        }
        return roaring_.cardinality() == count_;
    }

    [[nodiscard]] inline bool IsTrue(const u32 row_index) const {
        if (row_index >= count_) {
            UnrecoverableError("RoaringBitmap::IsTrue: row_index >= count_");
        }
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return true;
            }
        }
        return roaring_.contains(row_index);
    }

    inline void SetTrue(const u32 row_index) {
        if (row_index >= count_) {
            UnrecoverableError("RoaringBitmap::SetTrue: row_index >= count_");
        }
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return;
            }
        }
        roaring_.add(row_index);
    }

    inline void SetFalse(const u32 row_index) {
        if (row_index >= count_) {
            UnrecoverableError("RoaringBitmap::SetFalse: row_index >= count_");
        }
        if constexpr (init_all_true) {
            TryRemoveAllTrueFlag();
        }
        roaring_.remove(row_index);
    }

    inline void Set(const u32 row_index, const bool valid) { valid ? SetTrue(row_index) : SetFalse(row_index); }

    inline void SetTrueRange(const u32 start, const u32 end) {
        if (start >= end || end > count_) {
            UnrecoverableError("RoaringBitmap::SetTrueRange: start >= end || end > count_");
        }
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return;
            }
        }
        roaring_.addRange(start, end);
    }

    inline void SetFalseRange(const u32 start, const u32 end) {
        if (start >= end || end > count_) {
            UnrecoverableError("RoaringBitmap::SetFalseRange: start >= end || end > count_");
        }
        if constexpr (init_all_true) {
            TryRemoveAllTrueFlag();
        }
        roaring_.removeRange(start, end);
    }

    /**
     * Convert array and bitmap containers to run containers when it is more
     * efficient; also convert from run containers when more space efficient.
     * Returns true if the result has at least one run container.  Additional
     * savings might be possible by calling shrinkToFit().
     */
    inline void RunOptimize() {
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                // no need to optimize
                return;
            }
        }
        roaring_.runOptimize();
        if constexpr (init_all_true) {
            if (roaring_.cardinality() == count_) {
                SetAllTrue();
            }
        }
    }

    inline void SetAllTrue() {
        if constexpr (init_all_true) {
            all_true_flag_.value = true;
            roaring_ = roaring::Roaring();
        } else {
            roaring_.addRange(0, count_);
            roaring_.runOptimize();
        }
    }

    inline void SetAllFalse() {
        if constexpr (init_all_true) {
            all_true_flag_.value = false;
        }
        roaring_ = roaring::Roaring();
    }

    [[nodiscard]] inline u32 CountTrue() const {
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return count_;
            }
        }
        return roaring_.cardinality();
    }

    [[nodiscard]] inline u32 CountFalse() const {
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return 0;
            }
        }
        return count_ - CountTrue();
    }

    void MergeAnd(const RoaringBitmap &other) {
        if (count_ != other.count_) {
            UnrecoverableError("RoaringBitmap::MergeAnd: RoaringBitmaps have different sizes");
        }
        if constexpr (init_all_true) {
            if (other.all_true_flag_.value) {
                return;
            }
            if (all_true_flag_.value) {
                all_true_flag_.value = false;
                roaring_ = other.roaring_;
                return;
            }
        }
        roaring_ &= other.roaring_;
    }

    void MergeOr(const RoaringBitmap &other) {
        if (count_ != other.count_) {
            UnrecoverableError("RoaringBitmap::MergeOr: RoaringBitmaps have different sizes");
        }
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                return;
            }
            if (other.all_true_flag_.value) {
                SetAllTrue();
                return;
            }
        }
        roaring_ |= other.roaring_;
    }

    /**
     * Iterate over the bitmap elements. The function iterator is called once
     * for all the values with ptr (can be NULL) as the second parameter of
     * each call.
     *
     * roaring_iterator is simply a pointer to a function that returns bool
     * (true means that the iteration should continue while false means that it
     * should stop), and takes (uint32_t,void*) as inputs.
     */
    void RoaringBitmapApplyFunc(std::invocable<u32> auto &&func) const {
        if constexpr (init_all_true) {
            if (all_true_flag_.value) {
                for (u32 i = 0; i < count_; ++i) {
                    if (!func(i)) {
                        break;
                    }
                }
                return;
            }
        }
        auto roaring_iterator = [](u32 value, void *func_ptr) -> bool {
            auto &f = *static_cast<std::remove_reference_t<decltype(func)> *>(func_ptr);
            return f(value);
        };
        roaring_.iterate(roaring_iterator, &func);
    }

    // Estimated serialized size in bytes
    [[nodiscard]] i32 GetSizeInBytes() {
        // try to optimize the bitmap
        RunOptimize();
        i32 result_size = sizeof(count_);
        if constexpr (init_all_true) {
            result_size += sizeof(bool);
            if (all_true_flag_.value) {
                return result_size;
            }
        }
        result_size += sizeof(i32) + roaring_.getSizeInBytes();
        return result_size;
    }

    // Write to a char buffer
    void WriteAdv(char *&ptr) const {
        WriteBufAdv(ptr, count_);
        if constexpr (init_all_true) {
            const bool all_true_flag = all_true_flag_.value;
            WriteBufAdv(ptr, all_true_flag);
            if (all_true_flag) {
                return;
            }
        }
        const i32 size = roaring_.getSizeInBytes();
        WriteBufAdv(ptr, size);
        roaring_.write(ptr);
        ptr += size;
    }

    // Read from a serialized version
    static SharedPtr<RoaringBitmap> ReadAdv(const char *&ptr, i32 maxbytes) {
        if (maxbytes < 0) {
            UnrecoverableError("RoaringBitmap::ReadAdv: maxbytes < 0");
        }
        const u32 count = ReadBufAdv<u32>(ptr);
        maxbytes -= sizeof(u32);
        if constexpr (init_all_true) {
            const bool all_true_flag = ReadBufAdv<bool>(ptr);
            maxbytes -= sizeof(bool);
            if (all_true_flag) {
                return MakeSharedAllTrue(count);
            }
        }
        const i32 size = ReadBufAdv<i32>(ptr);
        maxbytes -= sizeof(i32);
        if (size > maxbytes) {
            UnrecoverableError("RoaringBitmap::ReadAdv: size > maxbytes");
        }
        auto bitmap = MakeShared<RoaringBitmap>(count);
        if constexpr (init_all_true) {
            bitmap->all_true_flag_.value = false;
        }
        bitmap->roaring_.read(ptr);
        ptr += size;
        // no need to reduce maxbytes
        return bitmap;
    }

    friend void swap(RoaringBitmap &left, RoaringBitmap &right) {
        using std::swap;
        swap(left.roaring_, right.roaring_);
        swap(left.count_, right.count_);
        if constexpr (init_all_true) {
            swap(left.all_true_flag_, right.all_true_flag_);
        }
    }

    bool operator==(const RoaringBitmap &other) const {
        if (count_ != other.count_) {
            return false;
        }
        const auto this_count_true = CountTrue();
        const auto other_count_true = other.CountTrue();
        if (this_count_true != other_count_true) {
            return false;
        }
        if (this_count_true == count_) {
            return true;
        }
        // now two bitmaps have the same count of true values, and it is less than count_
        return roaring_ == other.roaring_;
    }

private:
    void TryRemoveAllTrueFlag()
        requires init_all_true
    {
        if (all_true_flag_.value) {
            roaring_.addRange(0, count_);
            all_true_flag_.value = false;
        }
    }

    roaring::Roaring roaring_;
    u32 count_ = 0;
    OptionalAllTrueFlag<init_all_true> all_true_flag_;
};

export using Bitmask = RoaringBitmap<true>;
// export using Bitmap = RoaringBitmap<false>;

} // namespace infinity
