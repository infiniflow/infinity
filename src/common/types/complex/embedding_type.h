//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"
#include "main/logger.h"

#include <sstream>
#include <bitset>

namespace infinity {

enum EmbeddingDataType : i8 {
    kElemBit,
    kElemInt8,
    kElemInt16,
    kElemInt32,
    kElemInt64,
    kElemFloat,
    kElemDouble,
    kElemInvalid
};

struct EmbeddingType {
public:
    ptr_t ptr = nullptr;

    static size_t
            embedding_type_width[];
public:
    static inline size_t
    EmbeddingDataWidth(EmbeddingDataType type_index) {
        return embedding_type_width[type_index];
    }

    static inline size_t
    EmbeddingSize(EmbeddingDataType type, size_t dimension) {
        TypeAssert(type != EmbeddingDataType::kElemInvalid, "Invalid embedding data type");
        if(type == EmbeddingDataType::kElemBit) {
            size_t byte_count = (dimension + 7) >> 3;
            return byte_count;
        }

        return dimension * EmbeddingDataWidth(type);
    }

    static inline String
    EmbeddingDataType2String(EmbeddingDataType type) {
        switch(type) {
            case kElemBit:
                return "BIT";
            case kElemInt8:
                return "INT8";
            case kElemInt16:
                return "INT16";
            case kElemInt32:
                return "INT32";
            case kElemInt64:
                return "INT64";
            case kElemFloat:
                return "FLOAT32";
            case kElemDouble:
                return "FLOAT64";
            default: {
                TypeError("Unexpected embedding type");
            }
        }
    }

    static inline String
    Embedding2String(const EmbeddingType& embedding, EmbeddingDataType type, size_t dimension) {
        switch(type) {
            case kElemBit:
                return BitmapEmbedding2StringInternal(embedding, dimension);
            case kElemInt8:
                return Embedding2StringInternal<i8>(embedding, dimension);
            case kElemInt16:
                return Embedding2StringInternal<i16>(embedding, dimension);
            case kElemInt32:
                return Embedding2StringInternal<i32>(embedding, dimension);
            case kElemInt64:
                return Embedding2StringInternal<i64>(embedding, dimension);
            case kElemFloat:
                return Embedding2StringInternal<float>(embedding, dimension);
            case kElemDouble:
                return Embedding2StringInternal<double>(embedding, dimension);
            default: {
                TypeError("Unexpected embedding type");
            }
        }
    }

private:
    template<typename T>
    static inline String
    Embedding2StringInternal(const EmbeddingType& embedding, size_t dimension) {
        // TODO: High-performance implementation is needed here.
        std::stringstream ss;
        for(size_t i = 0; i < dimension - 1; ++i) {
            ss << ((T*)(embedding.ptr))[i] << ',';
        }
        ss << ((T*)(embedding.ptr))[dimension - 1];
        return ss.str();
    }

    static inline String
    BitmapEmbedding2StringInternal(const EmbeddingType& embedding, size_t dimension) {
        // TODO:  This is for bitmap, and high-performance implementation is needed here.
        std::stringstream ss;
        TypeAssert(dimension % 8 == 0, "Binary embedding dimension should be the times of 8.")

        i64* array = (i64*)(embedding.ptr);

        for(size_t i = 0; i < dimension / 8; ++i) {
            ss << std::bitset<8>(array[i]);
        }
        return ss.str();
    }
public:
    inline explicit
    EmbeddingType(ptr_t&& from_ptr) : ptr(from_ptr) {
        from_ptr = nullptr;
    }

    inline
    EmbeddingType(EmbeddingDataType type, size_t dimension) {
        size_t mem_size = EmbeddingSize(type, dimension);
        ptr = new char_t[mem_size]{0};
        GlobalResourceUsage::IncrRawMemCount();
    }

    inline
    ~EmbeddingType() {
        if(ptr != nullptr) {
            LOG_TRACE("Embedding type isn't null, need to manually SetNull or Reset");
            Reset();
        }
    }

    inline
    EmbeddingType(const EmbeddingType& other) = default;

    inline
    EmbeddingType(EmbeddingType&& other) noexcept {
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }

    EmbeddingType&
    operator=(const EmbeddingType& other) {
        if(this == &other)
            return *this;
        if(ptr != nullptr) {
            LOG_TRACE("Target embedding isn't null, need to manually SetNull or Reset");
//            Reset();
        }
        ptr = other.ptr;
        return *this;
    }

    EmbeddingType&
    operator=(EmbeddingType&& other) noexcept {
        if(this == &other)
            return *this;
        if(ptr != nullptr) {
            LOG_TRACE("Target embedding isn't null, need to manually SetNull or Reset");
//            Reset();
        }
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    void
    Init(const void* ptr, SizeT size);

    bool
    operator==(const EmbeddingType& other) const = delete;

    inline bool
    operator!=(const EmbeddingType& other) const = delete;

    inline void
    Reset() {
        if(ptr != nullptr) {
            delete[] ptr;
            ptr = nullptr;
            GlobalResourceUsage::DecrRawMemCount();
        }
    }

    inline void
    SetNull() {
        ptr = nullptr;
    }

    [[nodiscard]] inline String
    ToString() const {
        TypeError("ToString() isn't implemented");
    }
};

}

