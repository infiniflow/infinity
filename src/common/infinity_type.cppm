export module infinity_core:infinity_type;

import std.compat;

// Primitives

export {
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using idx_t = u64;

    using f32 = float;
    using f64 = double;

    // Transactions
    using TxnTimeStamp = uint64_t;
    using TransactionID = uint64_t;

    // Entry
    using SegmentID = uint32_t;
    using ChunkID = uint32_t;
    using BlockID = uint16_t;
    using ColumnID = uint64_t;

    // Related to entry
    using BlockOffset = uint16_t;
    using SegmentOffset = uint32_t;
}
