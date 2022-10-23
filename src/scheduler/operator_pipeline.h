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
    explicit OperatorPipeline(std::shared_ptr<PhysicalOperator> op);
    ~OperatorPipeline() override = default;

    static std::shared_ptr<OperatorPipeline> Create(const std::shared_ptr<PhysicalOperator>& op);

    std::shared_ptr<Table> GetResult() override;
protected:
    void
    OnExecute(std::shared_ptr<QueryContext>& query_context) final;

private:
    std::shared_ptr<PhysicalOperator> operator_;

};

}

