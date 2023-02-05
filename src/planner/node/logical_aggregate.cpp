//
// Created by jinhai on 23-2-5.
//

#include "logical_aggregate.h"

namespace infinity {

String
LogicalAggregate::ToString(i64 &space) {

    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str;

    if(!aggregates_.empty()) {
        ss << "Aggregate on: ";
        SizeT expression_count = aggregates_.size();
        for(SizeT i = 0; i < expression_count - 1; ++ i) {
            ss << aggregates_[i]->ToString() << ", ";
        }
        ss << aggregates_.back()->ToString();
    }

    if(!groups_.empty()) {
        if(aggregates_.empty()) {
            ss << "Group by: ";
        } else {
            ss << ", Group by: ";
        }
        SizeT expression_count = groups_.size();
        for(SizeT i = 0; i < expression_count - 1; ++ i) {
            ss << groups_[i]->ToString() << ", ";
        }
        ss << groups_.back()->ToString();
    }
    space += arrow_str.size();

    return ss.str();
}

}