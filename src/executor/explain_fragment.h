//
// Created by jinhai on 23-9-9.
//

#pragma once

#include "fragment/plan_fragment.h"

namespace infinity {

class ExplainFragment {
public:
    static void Explain(PlanFragment *op, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);
};

} // namespace infinity
