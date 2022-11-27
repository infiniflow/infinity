//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/varlen_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"
#include "main/logger.h"

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
        if (type == EmbeddingDataType::kElemBit) {
            size_t byte_count = (dimension + 7) >> 3;
            return byte_count;
        }

        return dimension * EmbeddingDataWidth(type);
    }
public:
    inline explicit
    EmbeddingType(ptr_t& from_ptr): ptr(from_ptr) {
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
        if(this == &other) return *this;
        if(ptr != nullptr) {
            LOG_TRACE("Target embedding isn't null, need to manually SetNull or Reset");
//            Reset();
        }
        ptr = other.ptr;
        return *this;
    }

    EmbeddingType&
    operator=(EmbeddingType&& other) noexcept {
        if(this == &other) return *this;
        if(ptr != nullptr) {
            LOG_TRACE("Target embedding isn't null, need to manually SetNull or Reset");
//            Reset();
        }
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

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
};

}

