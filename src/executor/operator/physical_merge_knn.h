//
// Created by jinhai on 23-10-1.
//


#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalMergeKnn final : public PhysicalOperator {
public:
    explicit
    PhysicalMergeKnn(u64 id,
                     SharedPtr<PhysicalOperator> left,
                     SharedPtr<Vector<String>> output_names,
                     SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                     u64 knn_table_index)
            : PhysicalOperator(PhysicalOperatorType::kMergeKnn, std::move(left), nullptr, id),
              output_names_(std::move(output_names)),
              output_types_(std::move(output_types)) {
    }

    ~PhysicalMergeKnn() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline u64
    knn_table_index() const {
        return knn_table_index_;
    }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

};

}
