//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "pipeline.h"
#include "executor/physical_operator.h"

namespace infinity {

//class PhysicalOperator;

class OperatorPipeline: public Pipeline {
public:
    explicit
    OperatorPipeline(SharedPtr<PhysicalOperator> op);

    ~OperatorPipeline() override = default;

    static SharedPtr<OperatorPipeline>
    Create(const SharedPtr<PhysicalOperator>& op);

    SharedPtr<Table>
    GetResult() override;

protected:
    void
    OnExecute(QueryContext* query_context) final;

private:
    SharedPtr<PhysicalOperator> operator_;
};

}

