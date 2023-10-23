//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include <string>
#include <cstdint>

namespace infinity {

struct VarcharType {
public:
    static constexpr size_t PREFIX_LENGTH = 6;
    static constexpr size_t INLINE_LENGTH = 14;
    static constexpr size_t LEN_LIMIT = 65536;

public:
    VarcharType() {
        length = 0;
        ptr = nullptr;
    }

    explicit VarcharType(const std::string &str);

    explicit VarcharType(const char *ptr);

    explicit VarcharType(const char *ptr, size_t len);

    ~VarcharType();

    VarcharType(const VarcharType &other);

    VarcharType(VarcharType &&other) noexcept;

    VarcharType &operator=(const VarcharType &other);

    VarcharType &operator=(VarcharType &&other) noexcept;

public:
    bool operator==(const VarcharType &other) const;

    bool operator>=(const VarcharType &other) const;

    bool operator>(const VarcharType &other) const;

    bool operator<=(const VarcharType &other) const;

    bool operator<(const VarcharType &other) const;

    [[nodiscard]] inline char* GetDataPtr() const {
        if (IsInlined()) {
            return (char*)(prefix);
        } else {
            return this->ptr;
        }
    }

    inline size_t GetDataLen() const { return this->length; }

public:
    void DeepCopy(const VarcharType &other);

    void Initialize(const std::string &str);

    void Initialize(const char *ptr);

    void Initialize(const char *ptr, size_t len);

    void InitializeAsEmptyStr();

    [[nodiscard]] inline bool IsInlined() const { return length <= INLINE_LENGTH; }

    void Reset(bool clean_memory = true);

    [[nodiscard]] std::string ToString() const;

    uint16_t length{0};                  // 65535 will be the limitation.
    char prefix[PREFIX_LENGTH]{}; // prefix of the varchar
    // If length <= 14, ptr will be used as prefix.
    char* ptr{nullptr}; // pointer to the varchar value buffer.
};

} // namespace infinity
