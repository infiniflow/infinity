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

class PlanFragment {
public:
    PlanFragment() = default;

    inline void
    AddOperator(const SharedPtr<PhysicalOperator>& op) {
        operators_.emplace_back(op);
    }

    inline void
    AddExchangeNode(const SharedPtr<PhysicalOperator>& op) {
        PlannerAssert(op->operator_type() == PhysicalOperatorType::kExchange, "Not physical exchange node");
        exchange_node_ = std::static_pointer_cast<PhysicalExchange>(op);
    }

    inline void
    AddSinkNode(const SharedPtr<PhysicalOperator>& op) {
        PlannerAssert(op->operator_type() == PhysicalOperatorType::kSink, "Not physical sink node");
        sink_node_ = std::static_pointer_cast<PhysicalSink>(op);
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
        return sink_node_->GetOutputNames();
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const {
        return sink_node_->GetOutputTypes();
    }

private:
    SharedPtr<PhysicalExchange> exchange_node_;
    SharedPtr<PhysicalSink> sink_node_;
    Vector<SharedPtr<PhysicalOperator>> operators_{};
    Vector<SharedPtr<DataBlock>> data_{};

    SharedPtr<PlanFragment> left_{};
    SharedPtr<PlanFragment> right_{};
};

}

