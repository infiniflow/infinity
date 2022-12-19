//
// Created by JinHai on 2022/11/6.
//

#include "common/types/internal_types.h"

namespace infinity {

struct BitmaskBuffer {
public:

    static SharedPtr<BitmaskBuffer>
    Make(size_t count);

    static SharedPtr<BitmaskBuffer>
    Make(const u64* input_ptr, size_t count);

    static constexpr const u64 UNIT_BITS = 64; // i64 bits count
    static constexpr const u64 BYTE_BITS = 8; // one byte bit count
    static constexpr const u64 UNIT_MAX = std::numeric_limits<u64>::max();
    static constexpr const u64 UNIT_MIN = 0;

public:
    BitmaskBuffer();
    ~BitmaskBuffer();

    void
    Initialize(size_t count);

    void
    Initialize(const u64* ptr, size_t count);

    static inline size_t
    UnitCount(size_t capacity) {
        return (capacity + (UNIT_BITS - 1)) / UNIT_BITS;
    }
public:
    UniquePtr<u64[]> data_ptr_ {nullptr};

    size_t count_ {0};
};

}
