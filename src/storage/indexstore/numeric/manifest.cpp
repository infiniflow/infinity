#include "manifest.h"
#include "common/utility/env.h"

#include <third_party/leveldb/util/crc32c.h>

#include <fstream>

// Manifest file format
// ====================
// [Header]
// Signature (2 bytes)
// Payload size (uint32; 4 bytes)
// CRC32C checksum of the payload (4 bytes)
//
// [Payload]
// Number of pages (uint32; 4 bytes)

namespace infinity {

using namespace leveldb;

void EncodeFixed32(char* dst, uint32_t value) {
    uint8_t* const buffer = reinterpret_cast<uint8_t*>(dst);

    // Recent clang and gcc optimize this to a single mov / str instruction.
    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
}

uint32_t DecodeFixed32(const char* ptr) {
    const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

    // Recent clang and gcc optimize this to a single mov / ldr instruction.
    return  (static_cast<uint32_t>(buffer[0])) |
            (static_cast<uint32_t>(buffer[1]) << 8) |
            (static_cast<uint32_t>(buffer[2]) << 16) |
            (static_cast<uint32_t>(buffer[3]) << 24);
}

void PutFixed32(std::string* dst, uint32_t value) {
    char buf[sizeof(value)];
    EncodeFixed32(buf, value);
    dst->append(buf, sizeof(buf));
}

const std::string kSignature = "IN";
constexpr size_t kHeaderSize = 10;
constexpr size_t kPayloadSizeOffset = 2;
constexpr size_t kChecksumOffset = 6;
constexpr size_t kPayloadOffset = kHeaderSize;

std::optional<Manifest> Manifest::LoadFrom(
    const std::filesystem::path& manifest_file, 
    Status& status) {

    std::ifstream in(manifest_file, std::ios_base::in | std::ios_base::binary);
    std::string buffer;
    buffer.resize(kHeaderSize);
    in.read(buffer.data(), kHeaderSize);
    if (in.fail()) {
        status = Status::IOError("Failed to read manifest header.");
        return std::optional<Manifest>();
    }

    Slice header(buffer);
    if (!header.starts_with(Slice(kSignature))) {
        status = Status::Corruption("Invalid manifest file signature.");
        return std::optional<Manifest>();
    }
    header.remove_prefix(kSignature.size());

    const uint32_t payload_size = DecodeFixed32(header.data());
    header.remove_prefix(4);
    const uint32_t expected_checksum = DecodeFixed32(header.data());
    header.remove_prefix(4);

    buffer.resize(payload_size);
    in.read(buffer.data(), payload_size);
    if (in.fail()) {
        status = Status::IOError("Failed to read manifest payload.");
        return std::optional<Manifest>();
    }


    const uint32_t computed_checksum = crc32c::Value(
      reinterpret_cast<const char*>(buffer.data()), payload_size);
    if (computed_checksum != expected_checksum) {
        status = Status::Corruption("Manifest checksum does not match.");
        return std::optional<Manifest>();
    }

    Slice payload(buffer);
    const uint32_t num_pages = DecodeFixed32(payload.data());
    payload.remove_prefix(4);

    status = Status::OK();
    return std::optional<Manifest>(Manifest(num_pages));
}

Status Manifest::WriteTo(const std::filesystem::path& manifest_file) const {
    std::string buffer;
    buffer.append(kSignature);
    PutFixed32(&buffer, 0);  // Payload length placeholder
    PutFixed32(&buffer, 0);  // Checksum placeholder

    PutFixed32(&buffer, num_pages_);
    const size_t payload_size = buffer.size() - kHeaderSize;
    EncodeFixed32(&buffer[kPayloadSizeOffset], payload_size);
    const uint32_t checksum = crc32c::Value(
      reinterpret_cast<const char*>(&buffer[kPayloadOffset]), payload_size);
    EncodeFixed32(&buffer[kChecksumOffset], checksum);

    {
        std::ofstream out(manifest_file, std::ios_base::out |
                                         std::ios_base::binary |
                                         std::ios_base::trunc);
        out.write(buffer.data(), buffer.size());
        out.flush();
        if (out.fail()) {
            return Status::IOError("Failed to write DB manifest");
        }
    }

    const int manifest_fd = open(manifest_file.c_str(), O_RDONLY);
    if (manifest_fd < 0) {
        return Status::IOError("Opening DB manifest:", manifest_file.string());
    }
    if (fdatasync(manifest_fd) < 0) {
        close(manifest_fd);
        return Status::IOError("Syncing DB manifest:", manifest_file.string());
    }
    close(manifest_fd);

    return Status::OK();
}
}