#pragma once

#include <leveldb/status.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>

namespace infinity {
using Status = leveldb::Status;

// Holds metadata
class Manifest {
public:
    class Builder;

    static std::optional<Manifest> LoadFrom(const std::filesystem::path& manifest_file, Status& status);

    Status WriteTo(const std::filesystem::path& manifest_file) const;

    uint64_t num_pages() const {
        return num_pages_;
    }
private:
    friend class Builder;
    Manifest(uint64_t num_pages)
        : num_pages_(num_pages) {}

    const uint64_t num_pages_;
};

class Manifest::Builder {
public:
    Builder() : num_pages_(0){}
    Builder&& WithNumPages(uint64_t num_pages) {
        num_pages_ = num_pages;
        return std::move(*this);
    }
    Manifest Build() && {
        return Manifest(num_pages_);
    }

private:
    uint64_t num_pages_;
};

}
