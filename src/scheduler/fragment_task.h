//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "executor/operator_state.h"
#include "storage/meta/entry/segment_entry.h"

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
    explicit FragmentTask(bool terminator = true) : is_terminator_(terminator) {}

    explicit FragmentTask(void *fragment_context, i64 task_id, i64 operator_count)
        : fragment_context_(fragment_context), task_id_(task_id), operator_count_(operator_count) {
        Init();
    }

    inline void SetTerminator() { is_terminator_ = true; }

    [[nodiscard]] inline bool IsTerminator() const { return is_terminator_; }

    InputState *GetInputStateByOperatorID(i64 operator_id) { return operator_input_state_[operator_id].get(); }

    OutputState *GetOutputStateByOperatorID(i64 operator_id) { return operator_output_state_[operator_id].get(); }

    // Set source
    // Scan source
    //    void
    //    AddSourceSegment(const SegmentEntry* segment_entry_ptr);
    //
    //    // Input queue
    //    void
    //    AddQueue(const BatchBlockingQueue);
    void Init();

    void OnExecute(i64 worker_id);

    [[nodiscard]] inline i64 LastWorkerID() const { return last_worker_id_; }

    [[nodiscard]] bool Ready() const;

    [[nodiscard]] bool Complete() const;

public:
    std::atomic<FragmentTaskState> state_{FragmentTaskState::kReady};

    UniquePtr<SourceState> source_state_;

    Vector<UniquePtr<InputState>> operator_input_state_;
    Vector<UniquePtr<OutputState>> operator_output_state_;

    UniquePtr<SinkState> sink_state_;

private:
    void *fragment_context_{};
    bool is_terminator_{false};
    i64 last_worker_id_{-1};
    i64 task_id_{-1};
    i64 operator_count_{0};
};

} // namespace infinity
