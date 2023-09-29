//
// Created by jinhai on 23-5-7.
//

#pragma once

#include "task.h"
#include "operator.h"
#include <iostream>
#include <cstdio>
#include <exception>
#include <cassert>

namespace infinity {
#if 0
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
#endif
enum class FragmentType {
    kParallel,
    kSerial,
    kInvalid
};

class Fragment {
public:
    inline explicit
    Fragment(u64 id, FragmentType type) : id_(id), fragment_type_(type) {}

    Vector<SharedPtr<Task>>
    BuildTask(u64 parallel_size);

    inline void
    AddOperator(UniquePtr<Operator> op) {
        operators_.emplace_back(std::move(op));
    }

    inline void
    SetChild(UniquePtr<Fragment> child) {
        child_ = std::move(child);
    }

    inline void
    AddSource(UniquePtr<Source> op) {
        source_ = std::move(op);
    }

    inline void
    AddSink(UniquePtr<Sink> op) {
        sink_ = std::move(op);
    }
private:
    u64 id_{};
    FragmentType fragment_type_{FragmentType::kInvalid};
    UniquePtr<Source> source_{};
    Vector<UniquePtr<Operator>> operators_{};
    UniquePtr<Sink> sink_{};
    UniquePtr<Fragment> child_{};
};

}
