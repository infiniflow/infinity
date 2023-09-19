//
// Created by jinhai on 23-4-27.
//

#pragma once

#include "main/query_context.h"
#include "executor/physical_operator.h"
#include "src/scheduler/fragment_context.h"
#include "executor/operator/physical_source.h"
#include "executor/operator/physical_sink.h"

#include <memory>

namespace infinity {

class PlanFragment {
public:
    PlanFragment() = default;

    explicit PlanFragment(u64 fragment_id) : fragment_id_(fragment_id) {}

    virtual
    ~PlanFragment() = default;

    void
    SetFragmentType(FragmentType fragment_type) {
        fragment_type_ = fragment_type;
    }

    [[nodiscard]] inline FragmentType
    GetFragmentType() const {
        return fragment_type_;
    }

    Vector<UniquePtr<FragmentTask>>&
    CreateTasks(QueryContext* query_context);

    inline void
    AddOperator(PhysicalOperator* op) {
        operators_.emplace_back(op);
    }

    inline Vector<PhysicalOperator*>&
    GetOperators() {
        return operators_;
    }

    void
    AddSourceNode(QueryContext* query_context,
                  SourceType source_type,
                  const SharedPtr<Vector<String>>& names,
                  const SharedPtr<Vector<SharedPtr<DataType>>>& types);

    void
    AddSinkNode(QueryContext* query_context,
                SinkType sink_type,
                const SharedPtr<Vector<String>>& names,
                const SharedPtr<Vector<SharedPtr<DataType>>>& types);

    [[nodiscard]] inline PhysicalSource*
    GetSourceNode() const {
        return source_.get();
    }

    [[nodiscard]] inline PhysicalSink*
    GetSinkNode() const {
        return sink_.get();
    }
    
    inline void
    AddChild(UniquePtr<PlanFragment> child_fragment) {
        child_fragment->parent_ = this;
        children_.emplace_back(std::move(child_fragment));
    }

    inline Vector<UniquePtr<PlanFragment>>&
    Children() {
        return children_;
    }

    inline PlanFragment*
    ChildPtr() const {
        return child_.get();
    }

    SharedPtr<Vector<String>>
    ToString();

    [[nodiscard]] inline u64
    FragmentID() const {
        return fragment_id_;
    }

    inline void
    SetContext(UniquePtr<FragmentContext> context) {
        context_ = std::move(context);
    }

    inline FragmentContext*
    GetContext() {
        return context_.get();
    }

    SharedPtr<Table>
    GetResult();

private:

    u64 fragment_id_{};

    UniquePtr<PhysicalSink> sink_{};

    Vector<PhysicalOperator*> operators_{};

    UniquePtr<PhysicalSource> source_{};

    PlanFragment* parent_{};

    UniquePtr<PlanFragment> child_{};

    Vector<UniquePtr<PlanFragment>> children_{};

    UniquePtr<FragmentContext> context_{};

    FragmentType fragment_type_{FragmentType::kSerialMaterialize};
};

}

