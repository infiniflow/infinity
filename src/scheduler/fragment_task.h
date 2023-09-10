//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "storage/meta/entry/segment_entry.h"
#include "executor/operator_state.h"

namespace infinity {


// Task type:
// DDL task
// DML task
// Query task
// Read data from queue or file system

enum class FragmentSourceType {
    kNone,
    kScan,
    kQueue,
};

enum class FragmentSinkType {
    kGlobalMaterialize,
    kLocalMaterialize,
    kStream,
};

enum class FragmentTaskState {
    kRunning,
    kCancelled,
    kFinished,
    kReady,
    kPending,
};

class FragmentTask {
public:
    explicit
    FragmentTask() = default;

    explicit
    FragmentTask(void* fragment_context,
                 FragmentSourceType source_type,
                 FragmentSinkType sink_type)
            : fragment_context_(fragment_context),
              source_type_(source_type),
              sink_type_(sink_type)
    {}

    explicit
    FragmentTask(void* fragment_context) : fragment_context_(fragment_context) {}

    inline void
    SetTerminator() {
        is_terminator_ = true;
    }

    inline bool
    IsTerminator() const {
        return is_terminator_;
    }

    inline void
    SetInputOutputState(InputState* input_state_ptr, OutputState* output_state_ptr) {
        input_state_ptr_ = input_state_ptr;
        output_state_ptr_ = output_state_ptr;
    }

    // Set source
    // Scan source
//    void
//    AddSourceSegment(const SegmentEntry* segment_entry_ptr);
//
//    // Input queue
//    void
//    AddQueue(const BatchBlockingQueue);

    void
    OnExecute(i64 worker_id);

    [[nodiscard]] inline i64
    LastWorkerID() const {
        return last_worker_id_;
    }

public:
    std::atomic<FragmentTaskState> state_{FragmentTaskState::kReady};

private:
    void* fragment_context_{};
    FragmentSourceType source_type_{FragmentSourceType::kScan};
    FragmentSinkType sink_type_{FragmentSinkType::kGlobalMaterialize};
    bool is_terminator_{false};
    i64 last_worker_id_{-1};
    InputState* input_state_ptr_{};
    OutputState* output_state_ptr_{};
};

}
