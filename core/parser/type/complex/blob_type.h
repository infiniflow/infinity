//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include <string>
#include <cstdint>

namespace infinity {

struct BlobType {
public:
    char* ptr{nullptr};
    uint64_t size{0}; // 4GB will be the limitation.

public:
    inline BlobType() = default;

    // The blob_ptr will also be freed by BlobType's destructor.
    inline BlobType(char* blob_ptr, uint64_t blob_size) : ptr(blob_ptr), size(blob_size) {}

    inline ~BlobType() { Reset(); }

    BlobType(const BlobType &other);

    BlobType(BlobType &&other) noexcept;

    BlobType &operator=(const BlobType &other);

    BlobType &operator=(BlobType &&other) noexcept;

    bool operator==(const BlobType &other) const;

    inline bool operator!=(const BlobType &other) const { return !operator==(other); }

public:
    void Copy(char* blob_ptr, uint64_t blob_size);

    void Move(char* blob_ptr, uint64_t blob_size);

    inline void Reset() {
        if (size != 0) {
            size = 0;

            delete[] ptr;

            ptr = nullptr;
        }
    }

    [[nodiscard]] inline std::string ToString() const { return {this->ptr, this->size}; }
};

} // namespace infinity
