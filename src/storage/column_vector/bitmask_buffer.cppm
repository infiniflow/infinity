//
// Created by jinhai on 23-10-17.
//

module;

import stl;

export module bitmask_buffer;

namespace infinity {

export struct BitmaskBuffer {
public:
    static SharedPtr<BitmaskBuffer> Make(SizeT count);

    static SharedPtr<BitmaskBuffer> Make(const u64 *input_ptr, SizeT count);

    static constexpr const u64 UNIT_BITS = 64; // i64 bits count
    static constexpr const u64 BYTE_BITS = 8;  // one byte bit count
    static constexpr const u64 UNIT_MAX = u64_max;
    static constexpr const u64 UNIT_MIN = 0;

public:
    BitmaskBuffer();

    ~BitmaskBuffer();

    void Initialize(SizeT count);

    void Initialize(const u64 *ptr, SizeT count);

    static inline SizeT UnitCount(SizeT capacity) { return (capacity + (UNIT_BITS - 1)) / UNIT_BITS; }

public:
    UniquePtr<u64[]> data_ptr_{nullptr};

    SizeT count_{0};
};

} // namespace infinity
