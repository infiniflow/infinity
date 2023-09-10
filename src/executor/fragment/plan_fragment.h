//
// Created by jinhai on 23-4-27.
//

#pragma once

#include "main/query_context.h"
#include "executor/physical_operator.h"
#include "src/scheduler/fragment_context.h"

#include <memory>

namespace infinity {

class PlanFragment {
public:
    PlanFragment() = default;
    virtual
    ~PlanFragment() = default;

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

    inline void
    SetSourceNode(PhysicalOperator* source_op) {
        source_ = source_op;
    }

    inline PhysicalOperator*
    GetSourceNode() const {
        return source_;
    }

    inline void
    SetSinkNode(PhysicalOperator* sink_op) {
        sink_ = sink_op;
    }

    inline PhysicalOperator*
    GetSinkNode() const {
        return sink_;
    }

//    [[nodiscard]] inline SharedPtr<Vector<String>>
//    GetOutputNames() const {
//        return sink_->GetOutputNames();
//    }
//
//    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
//    GetOutputTypes() const {
//        return sink_->GetOutputTypes();
//    }
    
    inline void
    AddDependency(PlanFragment* fragment) {
        dependencies_.push_back(fragment);
        fragment->parents_.push_back(this);
    }

    inline Vector<PlanFragment*>&
    Dependencies() {
        return dependencies_;
    }

    SharedPtr<Vector<String>>
    ToString();

    inline u64
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

    PhysicalOperator* source_{};

    Vector<PhysicalOperator*> operators_{};

    PhysicalOperator* sink_{};

    Vector<SharedPtr<DataBlock>> data_{};

    Vector<PlanFragment*> parents_{};

    Vector<PlanFragment*> dependencies_{};

    UniquePtr<FragmentContext> context_{};

};

}

