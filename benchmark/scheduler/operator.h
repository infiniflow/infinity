//
// Created by jinhai on 23-5-8.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

class Buffer {
public:
    explicit
    Buffer(SizeT size) : size_(size) {
        buffer_ = MakeUnique<char>(size);
    }

    inline void
    Append(const char *str) const {
        SizeT len = std::strlen(str);
        if (len + offset_ >= size_) {
            throw;
        }
        memcpy(buffer_.get() + offset_, str, len);
    }

    inline char *
    Get() const {
        return buffer_.get();
    }

private:
    UniquePtr<char> buffer_{nullptr};
    SizeT size_{};
    SizeT offset_{};
};

class Operator {
public:
    explicit
    Operator(const String &name) : op_name_(MakeUnique<String>(name)) {}

    inline void
    Run() {
        printf("Operator::Run(): %s, %s", op_name_->c_str(), input_buffer_->Get());
        output_buffer_->Append(input_buffer_->Get());
    }

    inline void
    SetInput(Buffer *buffer) {
        input_buffer_ = buffer;
    }

    inline void
    SetOutput(Buffer *buffer) {
        output_buffer_ = buffer;
    }

    [[nodiscard]] inline Buffer*
    OutputPtr() const {
        return output_buffer_;
    }

private:
    UniquePtr <String> op_name_;
    Buffer *input_buffer_{};
    Buffer *output_buffer_{};
};

class Sink {
public:
    explicit
    Sink(const String &name) : op_name_(MakeUnique<String>(name)) {}

    inline void
    Run() {
        printf("Sink::Run(): %s\n", op_name_->c_str());
        // Read all input buffer and send to output buffer
//        output_buffer_->Append(input_buffer_->Get());
    }

    inline void
    AddInput(Buffer *buffer) {
        input_buffers_.emplace_back(buffer);
    }

    inline void
    SetOutput(Buffer *buffer) {
        output_buffer_ = buffer;
    }

    [[nodiscard]] inline Buffer*
    OutputPtr() const {
        return output_buffer_;
    }

private:
    UniquePtr <String> op_name_;
    Vector<Buffer *> input_buffers_{};
    Buffer *output_buffer_{};
};

enum class SourceType {
    kScan, // each pipeline has its own source
    kExchange,  // all pipelines share one source
};

class Source {
public:
    explicit
    Source(const String &name, SourceType source_type) : op_name_(MakeUnique<String>(name)), type_(source_type) {}

    inline void
    Run() {
        // Send read file request to file reader
        printf("Source::Run(): %s\n", op_name_->c_str());

        // Or read data from the input buffer, and put it to output_buffer
    }

    inline void
    AddOutputPtr(Buffer **buffer_ptr) {
        output_buffer_ptrs_.emplace_back(buffer_ptr);
    }

    inline void
    SetInput(Buffer *buffer) {
        input_buffer_ = buffer;
    }

    SourceType
    type() const {
        return type_;
    }

    [[nodiscard]] inline Buffer*
    OutputPtr() const {
        return input_buffer_;
    }
private:
    UniquePtr <String> op_name_;
    Vector<Buffer **> output_buffer_ptrs_{};
    Buffer *input_buffer_{};
    SourceType type_;
};

}
