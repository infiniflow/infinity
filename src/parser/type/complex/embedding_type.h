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

#pragma once

#include "parser_assert.h"
#include <bitset>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <sstream>

namespace infinity {

enum EmbeddingDataType : int8_t {
    kElemBit,
    kElemInt8,
    kElemInt16,
    kElemInt32,
    kElemInt64,
    kElemFloat,
    kElemDouble,
    kElemInvalid,
};

template <typename T>
inline EmbeddingDataType ToEmbeddingDataType() {
    return EmbeddingDataType::kElemInvalid;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<bool>() {
    return EmbeddingDataType::kElemBit;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<int8_t>() {
    return EmbeddingDataType::kElemInt8;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<int16_t>() {
    return EmbeddingDataType::kElemInt16;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<int32_t>() {
    return EmbeddingDataType::kElemInt32;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<int64_t>() {
    return EmbeddingDataType::kElemInt64;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<float>() {
    return EmbeddingDataType::kElemFloat;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<double>() {
    return EmbeddingDataType::kElemDouble;
}

struct EmbeddingType {
public:
    char *ptr{};
    const bool new_allocated_{};

    static size_t embedding_type_width[];

public:
    static inline size_t EmbeddingDataWidth(EmbeddingDataType type_index) { return embedding_type_width[type_index]; }

    [[nodiscard]] static inline size_t EmbeddingSize(EmbeddingDataType type, size_t dimension) {
        ParserAssert(type != EmbeddingDataType::kElemInvalid, "Invalid embedding data type");
        if (type == EmbeddingDataType::kElemBit) {
            size_t byte_count = (dimension + 7) >> 3;
            return byte_count;
        }

        return dimension * EmbeddingDataWidth(type);
    }

    [[nodiscard]] static inline std::string EmbeddingDataType2String(EmbeddingDataType type) {
        switch (type) {
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
                ParserError("Unexpected embedding type");
            }
        }
        return std::string();
    }

    static EmbeddingDataType String2EmbeddingDataType(std::string_view sv) {
        if (sv == "BIT") {
            return kElemBit;
        } else if (sv == "INT8") {
            return kElemInt8;
        } else if (sv == "INT16") {
            return kElemInt16;
        } else if (sv == "INT32" || sv == "INT") {
            return kElemInt32;
        } else if (sv == "INT64") {
            return kElemInt64;
        } else if (sv == "FLOAT32" || sv == "FLOAT" || sv == "F32") {
            return kElemFloat;
        } else if (sv == "FLOAT64" || sv == "DOUBLE" || sv == "F64") {
            return kElemDouble;
        } else {
            ParserError("Unexpected embedding type");
        }
        return kElemInvalid;
    }

    [[nodiscard]] static inline std::string Embedding2String(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension) {
        switch (type) {
            case kElemBit:
                return BitmapEmbedding2StringInternal(embedding, dimension);
            case kElemInt8:
                return Embedding2StringInternal<int8_t>(embedding, dimension);
            case kElemInt16:
                return Embedding2StringInternal<int16_t>(embedding, dimension);
            case kElemInt32:
                return Embedding2StringInternal<int32_t>(embedding, dimension);
            case kElemInt64:
                return Embedding2StringInternal<int64_t>(embedding, dimension);
            case kElemFloat:
                return Embedding2StringInternal<float>(embedding, dimension);
            case kElemDouble:
                return Embedding2StringInternal<double>(embedding, dimension);
            default: {
                ParserError("Unexpected embedding type");
            }
        }
        return std::string();
    }

private:
    template <typename T>
    static inline std::string Embedding2StringInternal(const EmbeddingType &embedding, size_t dimension) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < dimension - 1; ++i) {
            ss << ((T *)(embedding.ptr))[i] << ',';
        }
        ss << ((T *)(embedding.ptr))[dimension - 1] << "]";
        return ss.str();
    }

    template <>
    inline std::string Embedding2StringInternal<float>(const EmbeddingType &embedding, size_t dimension) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < dimension - 1; ++i) {
            char buffer[20];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), ((float *)(embedding.ptr))[i], std::chars_format::general, 7);
            ss.write((const char *)buffer, ptr - buffer);
            ss.put(',');
        }
        char buffer[20];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), ((float *)(embedding.ptr))[dimension - 1], std::chars_format::general, 7);
        ss.write((const char *)buffer, ptr - buffer);
        ss << "]";
        return ss.str();
    }

    template <>
    inline std::string Embedding2StringInternal<double>(const EmbeddingType &embedding, size_t dimension) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < dimension - 1; ++i) {
            char buffer[20];
            auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), ((double *)(embedding.ptr))[i], std::chars_format::general, 16);
            ss.write((const char *)buffer, ptr - buffer);
            ss.put(',');
        }
        char buffer[20];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), ((double *)(embedding.ptr))[dimension - 1], std::chars_format::general, 16);
        ss.write((const char *)buffer, ptr - buffer);
        ss << "]";
        return ss.str();
    }

    [[nodiscard]] static inline std::string BitmapEmbedding2StringInternal(const EmbeddingType &embedding, size_t dimension) {
        // TODO:  This is for bitmap, and high-performance implementation is needed here.
        ParserAssert(dimension % 8 == 0, "Binary embedding dimension should be the times of 8.");
        std::string str;
        str.reserve(2 * dimension + 2);
        str.push_back('[');
        auto *array = reinterpret_cast<const uint8_t *>(embedding.ptr);
        for (size_t i = 0; i < dimension / 8; ++i) {
            const uint8_t byte = array[i];
            for (size_t j = 0; j < 8; ++j) {
                str.push_back((byte & (1 << j)) ? '1' : '0');
            }
        }
        str.push_back(']');
        return str;
    }

public:
    inline explicit EmbeddingType(char *&&from_ptr, bool new_alllocated) : ptr(from_ptr), new_allocated_(new_alllocated) { from_ptr = nullptr; }

    inline EmbeddingType(EmbeddingDataType type, size_t dimension) : new_allocated_(true) {
        size_t mem_size = EmbeddingSize(type, dimension);
        ptr = new char[mem_size]{0};
    }

    inline ~EmbeddingType() {
        if (ptr != nullptr) {
            Reset();
        }
    }

    inline EmbeddingType(const EmbeddingType &other) = default;

    inline EmbeddingType(EmbeddingType &&other) noexcept : new_allocated_(other.new_allocated_) {
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }

    EmbeddingType &operator=(const EmbeddingType &other) = delete;

    EmbeddingType &operator=(EmbeddingType &&other) = delete;

    void Init(const void *ptr, size_t size);

    bool operator==(const EmbeddingType &other) const = delete;

    inline bool operator!=(const EmbeddingType &other) const = delete;

    inline void Reset() {
        if (ptr && new_allocated_) {
            delete[] ptr;
        }
        ptr = nullptr;
    }

    inline void SetNull() { ptr = nullptr; }

    [[nodiscard]] inline std::string ToString() const {
        ParserError("ToString() isn't implemented");
        return std::string();
    }
};

} // namespace infinity
