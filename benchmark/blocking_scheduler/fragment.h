//
// Created by jinhai on 23-5-7.
//

#pragma once

#include <iostream>
#include <cstdio>
#include <exception>
#include <cassert>

#include "task.h"
#include "operator.h"

namespace infinity {
#if 0
class Pipeline {
public:
    inline explicit
    Pipeline(uint64_t fragment_id, uint64_t pipeline_id): fragment_id_(fragment_id), pipeline_id_(pipeline_id) {}

    inline void
    Add(std::unique_ptr<Operator> op) {
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
    std::vector<std::unique_ptr<Operator>> operators_{};
    std::vector<Buffer> buffers_{};
    uint64_t fragment_id_{};
    uint64_t pipeline_id_{};
};
#endif
enum class FragmentType {
    kParallel,
    kSerial,
    kInvalid
};

class Fragment {
public:
    inline explicit
    Fragment(uint64_t id, FragmentType type) : id_(id), fragment_type_(type) {}

    std::vector<std::shared_ptr<Task>>
    BuildTask(uint64_t parallel_size);

    inline void
    AddOperator(std::unique_ptr<Operator> op) {
        operators_.emplace_back(std::move(op));
    }

    inline void
    SetChild(std::unique_ptr<Fragment> child) {
        child_ = std::move(child);
    }

    inline void
    AddSource(std::unique_ptr<Source> op) {
        source_ = std::move(op);
    }

    inline void
    AddSink(std::unique_ptr<Sink> op) {
        sink_ = std::move(op);
    }
private:
    uint64_t id_{};
    FragmentType fragment_type_{FragmentType::kInvalid};
    std::unique_ptr<Source> source_{};
    std::vector<std::unique_ptr<Operator>> operators_{};
    std::unique_ptr<Sink> sink_{};
    std::unique_ptr<Fragment> child_{};
};

}
