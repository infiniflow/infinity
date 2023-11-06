//
// Created by jinhai on 23-11-5.
//

#pragma once

extern unsigned long strlen (const char *str);

enum class ErrorCode : long {

    kOk = 0,
    kNotFound,
    kNotImplemented,
    kReachQueryMemoryLimit,
    kNotSupportedCast,
    kAlreadyExist,
    kNetworkError,
    kServiceUnavailable,

};

class Status {
public:
    Status() = default;

    Status(ErrorCode code, const char *msg) : code_(code) {
        unsigned long len = strlen(msg);
        msg_ = new char[len]();
    }

    explicit Status(ErrorCode code) : code_(code) {}

    ~Status() {
        if(msg_ != nullptr) {
            delete[] msg_;
            msg_ = nullptr;
        }
    }

    Status(const Status &s);

    Status(Status &&s) noexcept;

    Status &operator=(Status &s);

    const Status &operator=(const Status &s);

    Status &operator=(Status &&s) noexcept;

    const Status &operator=(const Status &&s) noexcept;

    static Status OK() { return Status(); }

    bool ok() const { return code_ == ErrorCode::kOk && msg_ == nullptr; }

    ErrorCode code() const { return code_; }

    const char* message() const {
        return msg_;
    }

private:
    inline void CopyFrom(const Status &s);

    inline void MoveFrom(Status &s);

private:
    ErrorCode code_{ErrorCode::kOk};
    char* msg_{nullptr};
};

