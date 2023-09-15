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


public:

    explicit
    FragmentTask(bool terminator = true) : is_terminator_(terminator) {}

    explicit
    FragmentTask(void* fragment_context,
                 i64 task_id,
                 i64 operator_count) : fragment_context_(fragment_context), task_id_(task_id), operator_count_(operator_count) {
        Init();
    }

    inline void
    SetTerminator() {
        is_terminator_ = true;
    }

    [[nodiscard]] inline bool
    IsTerminator() const {
        return is_terminator_;
    }

    inline void
    SetInputOutputState(InputState* input_state_ptr, OutputState* output_state_ptr) {
        input_state_ptr_ = input_state_ptr;
        output_state_ptr_ = output_state_ptr;
    }

    InputState*
    GetInputStateByOperatorID(i64 operator_id) {
        return operator_input_state_[operator_id].get();
    }

    OutputState*
    GetOutputStateByOperatorID(i64 operator_id) {
        return operator_output_state_[operator_id].get();
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
    Init();

    void
    OnExecute(i64 worker_id);

    [[nodiscard]] inline i64
    LastWorkerID() const {
        return last_worker_id_;
    }

public:
    std::atomic<FragmentTaskState> state_{FragmentTaskState::kReady};

    UniquePtr<SourceState> source_state_;

    Vector<UniquePtr<InputState>> operator_input_state_;
    Vector<UniquePtr<OutputState>> operator_output_state_;

    UniquePtr<SinkState> sink_state_;

private:
    void* fragment_context_{};
    FragmentSourceType source_type_{FragmentSourceType::kScan};
    FragmentSinkType sink_type_{FragmentSinkType::kGlobalMaterialize};
    bool is_terminator_{false};
    i64 last_worker_id_{-1};
    InputState* input_state_ptr_{};
    OutputState* output_state_ptr_{};
    i64 task_id_{-1};
    i64 operator_count_{0};
};

}
