module;
import stl;
import third_party;
export module fst:error;

namespace infinity {

export String FormatBytes(u8 *bs_data, SizeT bs_len) {
    String output = "[";
    for (SizeT i = 0; i < bs_len; i++) {
        output += fmt::format("{:02X}", bs_data[i]);
    }
    output += "]";
    return output;
}

export enum FstErrorCode : long {
    kOk = 0, // success
    kVersion = 1001,
    kFormat = 1002,
    kChecksumMismatch = 1003,
    kChecksumMissing = 1004,
    kDuplicatedKey = 1005,
    kOutofOrderKey = 1006,
    kFromUtf8 = 1007,
    kUnsupported = 1008,
};

export class FstError {
public:
    FstErrorCode code_{FstErrorCode::kOk};
    UniquePtr<String> msg_{};

public:
    /// A version mismatch occurred while reading a finite state transducer.
    static FstError Version(u64 expected, u64 got);
    /// An unexpected error occurred while reading a finite state transducer.
    /// size - The number of bytes given to the FST constructor.
    static FstError Format(SizeT size);

    /// An error that is returned if verification of an FST fails because of a
    /// checksum mismatch.
    static FstError ChecksumMismatch(u32 expected, u32 got);

    /// An error that is returned if the caller attempts to verify an FST
    /// that does not have a checksum.
    static FstError ChecksumMissing();

    /// A duplicate key was inserted into a finite state transducer, which is
    /// not allowed.
    /// got - The duplicate key
    static FstError DuplicatedKey(u8 *got_data, SizeT got_len);

    /// A key was inserted out of order into a finite state transducer.
    /// Keys must always be inserted in lexicographic order.
    static FstError OutOfOrder(u8 *prev_data, SizeT prev_len, u8 *got_data, SizeT got_len);

    /// An error that occurred when trying to decode a UTF-8 byte key.
    static FstError FromUtf8(u8 *got_data, SizeT got_len, SizeT valid_up_to);

    /// An error that occurred when calling to unsupported methods.
    static FstError Unsupported(const String &method);

public:
    FstError() = default;

    inline explicit FstError(FstErrorCode code) : code_(code) {}

    inline FstError(FstErrorCode code, UniquePtr<String> message) : code_(code), msg_(std::move(message)) {}

    FstError(FstErrorCode code, const char *msg);

    FstError(FstError &s);

    FstError(FstError &&s) noexcept;

    FstError &operator=(FstError &s) noexcept;

    FstError &operator=(FstError &&s) noexcept;

    const FstError &operator=(const FstError &s) noexcept = delete;

    const FstError &operator=(const FstError &&s) noexcept = delete;

    [[nodiscard]] FstErrorCode code() const { return code_; }

    [[nodiscard]] const char *message() const {
        if (msg_.get() != nullptr) {
            return msg_->c_str();
        } else {
            return nullptr;
        }
    }

    void MoveError(FstError &e);
    void MoveError(FstError &&e);
};

} // namespace infinity
