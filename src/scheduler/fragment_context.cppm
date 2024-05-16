// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import fragment_task;
import query_context;
import profiler;
import physical_operator;
import physical_source;
import physical_sink;
import data_table;
import data_block;
import knn_scan_data;
import create_index_data;
import logger;
import third_party;

export module fragment_context;

namespace infinity {

class PlanFragment;
export class FragmentContext;

export enum class FragmentType {
    kInvalid,
    kSerialMaterialize,
    kParallelMaterialize,
    kParallelStream,
};

export String FragmentType2String(FragmentType type) {
    switch (type) {
        case FragmentType::kInvalid:
            return String("Invalid");
        case FragmentType::kSerialMaterialize:
            return String("SerialMaterialize");
        case FragmentType::kParallelMaterialize:
            return String("ParallelMaterialize");
        case FragmentType::kParallelStream:
            return String("ParallelStream");
    }
}

export class Notifier {
    SizeT all_task_n_ = 0;
    SizeT start_task_n_ = 0;
    bool error_ = false;
    FragmentContext *error_fragment_ctx_ = nullptr;

    std::mutex locker_{};
    std::condition_variable cv_{};

    bool Check() const { return all_task_n_ == 0 || (error_ && start_task_n_ == 0); }

public:
    void SetTaskN(SizeT all_task_n) { all_task_n_ = all_task_n; }

    void Wait() {
        std::unique_lock<std::mutex> lk(locker_);
        cv_.wait(lk, [&] { return this->Check(); });
    }

    bool StartTask() {
        std::unique_lock<std::mutex> lk(locker_);
        if (!error_) {
            ++start_task_n_;
            return true;
        }
        return false;
    }

    void FinishTask(bool error, FragmentContext *fragment_ctx) {
        std::unique_lock<std::mutex> lk(locker_);
        if (error && !error_) {
            error_fragment_ctx_ = fragment_ctx;
            error_ = true;
        }
        --start_task_n_;
        --all_task_n_;
        if (this->Check()) {
            cv_.notify_one();
        }
    }

    void UnstartTask() {
        std::unique_lock<std::mutex> lk(locker_);
        --start_task_n_;
        if (this->Check()) {
            cv_.notify_one();
        }
    }

    FragmentContext *error_fragment_ctx() const { return error_fragment_ctx_; }
};

export class FragmentContext {
public:
    static void BuildTask(QueryContext *query_context, FragmentContext *parent_context, PlanFragment *fragment_ptr, Notifier *notifier);

public:
    explicit FragmentContext(PlanFragment *fragment_ptr, QueryContext *query_context, Notifier *notifier);

    virtual ~FragmentContext() = default;

    inline void IncreaseTask() { unfinished_task_n_.fetch_add(1); }

    inline void FlushProfiler(TaskProfiler &profiler) {
        if (!query_context_->is_enable_profiling()) {
            return;
        }
        query_context_->FlushProfiler(std::move(profiler));
    }

    bool TryFinishFragment();

    Vector<PhysicalOperator *> &GetOperators();

    [[nodiscard]] PhysicalSink *GetSinkOperator() const;

    [[nodiscard]] PhysicalSource *GetSourceOperator() const;

    void CreateTasks(i64 parallel_count, i64 operator_count);

    inline Vector<UniquePtr<FragmentTask>> &Tasks() { return tasks_; }

    [[nodiscard]] inline bool IsMaterialize() const {
        return fragment_type_ == FragmentType::kSerialMaterialize || fragment_type_ == FragmentType::kParallelMaterialize;
    }

    inline SharedPtr<DataTable> GetResult() {
        notifier_->Wait();

        if (notifier_->error_fragment_ctx() != nullptr) {
            return notifier_->error_fragment_ctx()->GetResultInternal();
        }

        return GetResultInternal();
    }

    inline QueryContext *query_context() { return query_context_; }

    inline PlanFragment *fragment_ptr() { return fragment_ptr_; }

    [[nodiscard]] inline FragmentType ContextType() const { return fragment_type_; }

    void DumpFragmentCtx();

    Notifier *notifier() { return notifier_; }

private:
    bool TryStartFragment() {
        u64 unfinished_child = unfinished_child_n_.fetch_sub(1);
        return unfinished_child == 1;
    }

    bool TryFinishFragmentInner() {
        u64 unfinished_task = unfinished_task_n_.fetch_sub(1);
        return unfinished_task == 1;
    }

    void MakeSourceState(i64 parallel_count);

    void MakeSinkState(i64 parallel_count);

protected:
    virtual SharedPtr<DataTable> GetResultInternal() = 0;

protected:
    Notifier *notifier_{};

    PlanFragment *fragment_ptr_{};

    QueryContext *query_context_{};

    Vector<UniquePtr<FragmentTask>> tasks_{};

    Vector<SharedPtr<DataBlock>> data_array_{};

    FragmentType fragment_type_{FragmentType::kInvalid};

    atomic_u64 unfinished_task_n_{0};
    atomic_u64 unfinished_child_n_{0};
};

export class SerialMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit inline SerialMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context, Notifier *notifier)
        : FragmentContext(fragment_ptr, query_context, notifier) {}

    ~SerialMaterializedFragmentCtx() final = default;

    SharedPtr<DataTable> GetResultInternal() final;

public:
    UniquePtr<KnnScanSharedData> knn_scan_shared_data_{};
};

export class ParallelMaterializedFragmentCtx final : public FragmentContext {
public:
    explicit inline ParallelMaterializedFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context, Notifier *notifier)
        : FragmentContext(fragment_ptr, query_context, notifier) {}

    ~ParallelMaterializedFragmentCtx() final = default;

    SharedPtr<DataTable> GetResultInternal() final;

public:
    UniquePtr<KnnScanSharedData> knn_scan_shared_data_{};

    UniquePtr<CreateIndexSharedData> create_index_shared_data_{};

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

export class ParallelStreamFragmentCtx final : public FragmentContext {
public:
    explicit inline ParallelStreamFragmentCtx(PlanFragment *fragment_ptr, QueryContext *query_context, Notifier *notifier)
        : FragmentContext(fragment_ptr, query_context, notifier) {}

    ~ParallelStreamFragmentCtx() final = default;

    SharedPtr<DataTable> GetResultInternal() final;

protected:
    HashMap<u64, Vector<SharedPtr<DataBlock>>> task_results_{};
};

} // namespace infinity
