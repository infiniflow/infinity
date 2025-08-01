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
#include "type/number/bfloat16.h"
#include "type/number/float16.h"
#include <arrow/type_fwd.h>
#include <bitset>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>

namespace infinity {

enum class EmbeddingDataType : int8_t {
    kElemBit,
    kElemInt8,
    kElemInt16,
    kElemInt32,
    kElemInt64,
    kElemFloat,
    kElemDouble,
    kElemUInt8,
    kElemFloat16,
    kElemBFloat16,
    kElemInvalid,
};

bool operator==(const EmbeddingDataType &type, const arrow::Type::type &arrow_type);

bool operator!=(const EmbeddingDataType &type, const arrow::Type::type &arrow_type);

constexpr auto to_underlying_val(EmbeddingDataType type) { return static_cast<std::underlying_type_t<EmbeddingDataType>>(type); }

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

template <>
inline EmbeddingDataType ToEmbeddingDataType<uint8_t>() {
    return EmbeddingDataType::kElemUInt8;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<float16_t>() {
    return EmbeddingDataType::kElemFloat16;
}

template <>
inline EmbeddingDataType ToEmbeddingDataType<bfloat16_t>() {
    return EmbeddingDataType::kElemBFloat16;
}

struct EmbeddingType {
public:
    char *ptr{};
    bool new_allocated_{};

    static size_t embedding_type_width[];

    void Own(EmbeddingDataType type, size_t dimension) {
        if (new_allocated_) {
            return;
        }
        size_t mem_size = EmbeddingSize(type, dimension);
        char *new_ptr = new char[mem_size];
        std::memcpy(new_ptr, ptr, mem_size);
        ptr = new_ptr;
        new_allocated_ = true;
    }

public:
    static inline size_t EmbeddingDataWidth(EmbeddingDataType type_index) { return embedding_type_width[to_underlying_val(type_index)]; }

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
            case EmbeddingDataType::kElemBit:
                return "BIT";
            case EmbeddingDataType::kElemInt8:
                return "INT8";
            case EmbeddingDataType::kElemInt16:
                return "INT16";
            case EmbeddingDataType::kElemInt32:
                return "INT32";
            case EmbeddingDataType::kElemInt64:
                return "INT64";
            case EmbeddingDataType::kElemFloat:
                return "FLOAT32";
            case EmbeddingDataType::kElemDouble:
                return "FLOAT64";
            case EmbeddingDataType::kElemUInt8:
                return "UINT8";
            case EmbeddingDataType::kElemFloat16:
                return "FLOAT16";
            case EmbeddingDataType::kElemBFloat16:
                return "BFLOAT16";
            case EmbeddingDataType::kElemInvalid: {
                ParserError("Unexpected embedding type");
            }
        }
        return {};
    }

    static EmbeddingDataType String2EmbeddingDataType(std::string_view sv) {
        if (sv == "BIT") {
            return EmbeddingDataType::kElemBit;
        } else if (sv == "INT8" || sv == "I8") {
            return EmbeddingDataType::kElemInt8;
        } else if (sv == "INT16" || sv == "I16") {
            return EmbeddingDataType::kElemInt16;
        } else if (sv == "INT32" || sv == "I32" || sv == "INT" || sv == "INTEGER") {
            return EmbeddingDataType::kElemInt32;
        } else if (sv == "INT64") {
            return EmbeddingDataType::kElemInt64;
        } else if (sv == "FLOAT32" || sv == "FLOAT" || sv == "F32") {
            return EmbeddingDataType::kElemFloat;
        } else if (sv == "FLOAT64" || sv == "DOUBLE" || sv == "F64") {
            return EmbeddingDataType::kElemDouble;
        } else if (sv == "UINT8" || sv == "U8") {
            return EmbeddingDataType::kElemUInt8;
        } else if (sv == "FLOAT16" || sv == "F16") {
            return EmbeddingDataType::kElemFloat16;
        } else if (sv == "BFLOAT16" || sv == "BF16") {
            return EmbeddingDataType::kElemBFloat16;
        } else {
            ParserError("Unexpected embedding type");
        }
        return EmbeddingDataType::kElemInvalid;
    }

    [[nodiscard]] static inline std::string Embedding2String(const EmbeddingType &embedding, EmbeddingDataType type, size_t dimension) {
        switch (type) {
            case EmbeddingDataType::kElemBit:
                return BitmapEmbedding2StringInternal(embedding, dimension);
            case EmbeddingDataType::kElemInt8:
                return Embedding2StringInternal<int8_t>(embedding, dimension);
            case EmbeddingDataType::kElemInt16:
                return Embedding2StringInternal<int16_t>(embedding, dimension);
            case EmbeddingDataType::kElemInt32:
                return Embedding2StringInternal<int32_t>(embedding, dimension);
            case EmbeddingDataType::kElemInt64:
                return Embedding2StringInternal<int64_t>(embedding, dimension);
            case EmbeddingDataType::kElemFloat:
                return Embedding2StringInternal<float>(embedding, dimension);
            case EmbeddingDataType::kElemDouble:
                return Embedding2StringInternal<double>(embedding, dimension);
            case EmbeddingDataType::kElemUInt8:
                return Embedding2StringInternal<uint8_t>(embedding, dimension);
            case EmbeddingDataType::kElemFloat16:
                return Embedding2StringInternal<float16_t>(embedding, dimension);
            case EmbeddingDataType::kElemBFloat16:
                return Embedding2StringInternal<bfloat16_t>(embedding, dimension);
            case EmbeddingDataType::kElemInvalid: {
                ParserError("Unexpected embedding type");
            }
        }
        return {};
    }

private:
    template <typename T>
    static inline std::string Embedding2StringInternal(const EmbeddingType &embedding, size_t dimension) {
        static_assert(!std::is_same_v<T, bool>, "bit embedding is not supported in this function.");
        auto *val_ptr = reinterpret_cast<T *>(embedding.ptr);
        std::stringstream ss;
        ss << '[';
        for (size_t i = 0; i < dimension; ++i) {
            if (i) {
                ss << ',';
            }
            if constexpr (std::is_same_v<T, float16_t> || std::is_same_v<T, bfloat16_t> || std::is_same_v<T, float> || std::is_same_v<T, double>) {
                // Need to cast float16, bfloat16 to float to apply std::to_chars.
                using FormatFloatType = std::conditional_t<std::is_same_v<T, double>, double, float>;
                char buffer[30];
                auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), static_cast<FormatFloatType>(val_ptr[i]));
                ss.write(static_cast<const char *>(buffer), ptr - buffer);
            } else {
                // Need to cast i8, u8 to int to avoid printing as character.
                using FormatIntType = std::conditional_t<(std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>), int, T>;
                ss << static_cast<FormatIntType>(val_ptr[i]);
            }
        }
        ss << ']';
        return std::move(ss).str();
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
        // This is a simplified implementation that cannot determine the exact type and dimension
        // without additional context. In practice, you would need to pass the EmbeddingDataType
        // and dimension as parameters or store them as member variables.
        if (ptr == nullptr) {
            return "NULL";
        }
        return "[EMBEDDING_DATA]";
    }

    bool Eq(const EmbeddingType &other, EmbeddingDataType type, size_t dimension) const {
        return std::memcmp(ptr, other.ptr, EmbeddingSize(type, dimension)) == 0;
    }
};

} // namespace infinity
