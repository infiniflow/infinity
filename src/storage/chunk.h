//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "data_type.h"
#include "table.h"

namespace infinity {

// Chunk is an column block with a row group. A Chunk contains specific type of data and nullable flags.
// Two types of chunk: fixed row count chunk and fixed size chunk.

template<typename T>
class Chunk {
public:
    // Third parameter means row count limitation when chunk type is Fixed Row Count Chunk.
    // Third parameter means size limitation when chunk type is Fixed Chunk Size.
    explicit Chunk(TableType type, LogicalType data_type, uint64_t number = 0)
            : type_(type), data_type_(data_type), limit_(number) {} ;
    virtual ~Chunk() = default;

    [[nodiscard]] LogicalType DataType() const { return data_type_; }

    [[nodiscard]] uint64_t row_count() const { return row_count_; }
    [[nodiscard]] uint64_t memory_size() const { return row_count_ * sizeof(T); }

    void Append(const Chunk<T>& chunk, uint64_t start_idx);

protected:
    TableType type_;
    LogicalType data_type_;
    uint64_t limit_{0};
    uint64_t row_count_{0};
    uint64_t chunk_size_{0};
    std::vector<T> data_;
};

// TODO: unit test !!!
template<typename T>
void
Chunk<T>::Append(const Chunk<T>& chunk, uint64_t start_idx) {
    uint64_t available_size = limit_ - row_count_;
    uint64_t insert_size = chunk.data_.size() - start_idx;
    if(available_size >= insert_size) {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.end());
    } else {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.begin() + available_size);
    }
}

/*
class ChunkBoolean : public Chunk {
public:
    explicit ChunkBoolean(ChunkType type, uint64_t number = 0) : Chunk(type, PhysicalType::kBool, number) {};
    [[nodiscard]] uint64_t memory_size() const override { return sizeof(BooleanT) * row_count_; }
private:
    std::vector<BooleanT> data_;
};

class ChunkTinyInt : public Chunk {
public:
    explicit ChunkTinyInt(ChunkType type, uint64_t number = 0) : Chunk(type, PhysicalType::kInt8, number) {};
    [[nodiscard]] uint64_t memory_size() const override { return sizeof(TinyIntT) * row_count_; }
private:
    std::vector<TinyIntT> data_;
};

class ChunkSmallIntT : public Chunk {
public:
    explicit ChunkSmallIntT(ChunkType type, uint64_t number = 0) : Chunk(type, PhysicalType::kInt16, number) {};
    [[nodiscard]] uint64_t memory_size() const override { return sizeof(SmallIntT) * row_count_; }
private:
    std::vector<SmallIntT> data_;
};

template<> class Chunk<BooleanT>;
template<> class Chunk<TinyIntT>;
template<> class Chunk<SmallIntT>;
template<> class Chunk<IntegerT>;
template<> class Chunk<BigIntT>;
template<> class Chunk<FloatT>;
template<> class Chunk<DoubleT>;
template<> class Chunk<DateT>;
template<> class Chunk<TimeT>;
template<> class Chunk<DateTimeT>;
template<> class Chunk<IntervalT>;
template<> class Chunk<VarcharT>;
template<> class Chunk<VarcharT>;
template<> class Chunk<TextT>;
*/

}
