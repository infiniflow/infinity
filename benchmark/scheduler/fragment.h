//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "task.h"
#include <iostream>
#include <cstdio>
#include <exception>

namespace infinity {

class Buffer {
public:
    explicit
    Buffer(SizeT size): size_(size) {
        buffer_ = MakeUnique<char>(size);
    }

    inline void
    Append(const char* str) const {
        SizeT len = std::strlen(str);
        if(len + offset_ >= size_) {
            throw ;
        }
        memcpy(buffer_.get() + offset_, str, len);
    }

    inline char*
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
    Operator(const String& name) : op_name_(MakeUnique<String>(name)) {}

    inline void
    Run() {
        printf("Operator::Run(): %s, %s", op_name_->c_str(), input_buffer_->Get());
        output_buffer_->Append(input_buffer_->Get());
    }

    inline void
    SetInput(Buffer* buffer) {
        input_buffer_ = buffer;
    }

    inline void
    SetOutput(Buffer* buffer) {
        output_buffer_ = buffer;
    }
private:
    UniquePtr<String> op_name_;
    Buffer* input_buffer_{};
    Buffer* output_buffer_{};
};

class Sink {
public:
    explicit
    Sink(const String& name) : op_name_(MakeUnique<String>(name)) {}

    inline void
    Run() {
        printf("Sink::Run(): %s\n", op_name_->c_str());
        // Read all input buffer and send to output buffer
//        output_buffer_->Append(input_buffer_->Get());
    }

    inline void
    AddInput(Buffer* buffer) {
        input_buffers_.emplace_back(buffer);
    }

    inline void
    SetOutput(Buffer* buffer) {
        output_buffer_ = buffer;
    }
private:
    UniquePtr<String> op_name_;
    Vector<Buffer*> input_buffers_{};
    Buffer* output_buffer_{};
};

class Source {
public:
    explicit
    Source(const String& name) : op_name_(MakeUnique<String>(name)) {}

    inline void
    Run() {
        // Send read file request to file reader
        printf("Source::Run(): %s\n", op_name_->c_str());

        // Or read data from the input buffer, and put it to output_buffer
    }

    inline void
    AddOutputPtr(Buffer** buffer_ptr) {
        output_buffer_ptrs_.emplace_back(buffer_ptr);
    }

    inline void
    SetInput(Buffer* buffer) {
        input_buffer_ = buffer;
    }
private:
    UniquePtr<String> op_name_;
    Vector<Buffer**> output_buffer_ptrs_{};
    Buffer* input_buffer_{};
};

class Pipeline {
public:
    inline explicit
    Pipeline(u64 fragment_id, u64 pipeline_id): fragment_id_(fragment_id), pipeline_id_(pipeline_id) {}

    inline void
    Add(UniquePtr<Operator> op) {
        operators_.emplace_back(std::move(op));

        // 128 bytes buffer for the operators input
        buffers_.emplace_back(128);

        SizeT last_index = operators_.size() - 1;
        operators_[last_index]->SetInput(&buffers_[last_index]);
        if(last_index > 0) {
            operators_[last_index]->SetOutput(&buffers_[last_index - 1]);
        }
    }

    [[nodiscard]] inline SizeT
    size() const {
        return operators_.size();
    }

    inline void
    ConnectBuffer(Buffer* buffer) {
        operators_[0]->SetInput(buffer);
    }

    inline void
    PrintOutput() const {
        printf("Pipeline Output: %s", buffers_.back().Get());
    }

private:
    Vector<UniquePtr<Operator>> operators_{};
    Vector<Buffer> buffers_{};
    u64 fragment_id_{};
    u64 pipeline_id_{};
};

class Fragment {
public:
    inline static UniquePtr<Task>
    BuildTaskFromFragment(const UniquePtr<Fragment>& fragment) {
        UniquePtr<Task> child_task = fragment->child_ != nullptr ? BuildTaskFromFragment(fragment->child_) : nullptr;


    }

public:
    inline explicit
    Fragment(u64 id): id_(id) {}

    inline void
    AddOperator(UniquePtr<Operator> op) {
        operators_.emplace_back(std::move(op));
    }

    inline void
    SetChild(UniquePtr<Fragment> child) {
        child_ = std::move(child);
    }
private:
    u64 id_{};
    UniquePtr<Source> source{};
    Vector<UniquePtr<Operator>> operators_{};
    UniquePtr<Sink> sink_{};
    UniquePtr<Fragment> child_{};
};

}
