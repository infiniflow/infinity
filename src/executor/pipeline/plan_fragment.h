//
// Created by jinhai on 23-4-27.
//

#pragma once

#include "main/query_context.h"
#include "executor/physical_operator.h"
#include "executor/operator/physical_exchange.h"
#include "executor/operator/physical_sink.h"

#include <memory>

namespace infinity {

class PlanFragment :public std::enable_shared_from_this<PlanFragment>{
public:
    PlanFragment() = default;

    inline void
    AddOperator(const SharedPtr<PhysicalOperator>& op) {
        operators_.emplace_back(op);
    }

    inline void
    AddSourceNode(const SharedPtr<PhysicalOperator>& op) {
        source_ = std::static_pointer_cast<PhysicalOperator>(op);
    }

    inline void
    AddSinkNode(const SharedPtr<PhysicalOperator>& op) {
        sink_ = std::static_pointer_cast<PhysicalOperator>(op);
    }

    inline void
    AddLeft(const SharedPtr<PlanFragment>& left) {
        left_ = left;
    }

    inline void
    AddRight(const SharedPtr<PlanFragment>& right) {
        left_ = right;
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const {
        return sink_->GetOutputNames();
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const {
        return sink_->GetOutputTypes();
    }
    
    inline void
    AddDependency(SharedPtr<PlanFragment>& fragment) {
        dependencies.push_back(SharedPtr<PlanFragment>(fragment));
        fragment->parents.push_back(SharedPtr<PlanFragment>(shared_from_this()));
    }

    SharedPtr<Vector<String>>
    ToString();


private:

    /// The source of this fragment
    SharedPtr<PhysicalOperator> source_;
    /// The chain of intermediate operators
    Vector<SharedPtr<PhysicalOperator>> operators_{};
    /// The sink (i.e. destination) for data;
    SharedPtr<PhysicalOperator> sink_;
    /// The data blocks that are produced by this fragment
    Vector<SharedPtr<DataBlock>> data_{};
    /// The parent fragments (i.e. pipelines that are dependent on this pipeline to finish)
    Vector<SharedPtr<PlanFragment>> parents;
    /// The dependencies of this fragment
    Vector<SharedPtr<PlanFragment>> dependencies;

    /// may be not necessary
    SharedPtr<PlanFragment> left_{};
    SharedPtr<PlanFragment> right_{};
};

}

