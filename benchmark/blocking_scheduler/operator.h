//
// Created by jinhai on 23-5-8.
//

#pragma once

#include "buffer.h"
#include "buffer_queue.h"

namespace infinity {

class Operator {
public:
    explicit
    Operator(const std::string& name) : op_name_(std::make_unique<std::string>(name)) {}

    inline void
    Run(const Buffer* input_buffer, Buffer* output_buffer) {
//        printf("Operator::Run(): %s\n", op_name_->c_str());
    }

private:
    std::unique_ptr<std::string> op_name_;
};

class Sink {
public:
    explicit
    Sink(const std::string& name) : op_name_(std::make_unique<std::string>(name)) {}

    inline void
    Run(const Buffer* input_buffer, std::vector<ConcurrentQueue*>& output_buffers) {
//        printf("Sink::Run(): %s\n", op_name_->c_str());
        // Read all input buffer and send to output buffer
//        output_buffer_->Append(input_buffer_->Get());
    }
private:
    std::unique_ptr<std::string> op_name_;
};

enum class SourceType {
    kScan, // each pipeline has its own source
    kExchange,  // all pipelines share one source
};

class Source {
public:
    explicit
    Source(const std::string& name, SourceType source_type) : op_name_(std::make_unique<std::string>(name)), type_(source_type) {}

    inline void
    Run(ConcurrentQueue* input_queue, const Buffer* input_buffer, Buffer* output_buffer) {
        // Send read file request to file reader
//        printf("Source::Run(): %s\n", op_name_->c_str());

        // Or read data from the input buffer, and put it to output_buffer
    }

    SourceType
    type() const {
        return type_;
    }
private:
    std::unique_ptr<std::string> op_name_;
    SourceType type_;
};

}
