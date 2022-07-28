//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "pipeline.h"
//#include "executor/physical_operator.h"

namespace infinity {

class PhysicalOperator;

class OperatorPipeline: public Pipeline {
public:
    explicit OperatorPipeline(std::shared_ptr<PhysicalOperator> op);
    ~OperatorPipeline() override = default;

protected:
    void OnExecute() final;

private:
    std::shared_ptr<PhysicalOperator> operator_;

};

}

