//
// Created by jinhai on 23-2-5.
//

#include "logical_aggregate.h"

namespace infinity {

Vector<ColumnBinding>
LogicalAggregate::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = groups_.size();
    result.reserve(groups_count + aggregates_count);
    for(SizeT i = 0; i < groups_count; ++ i) {
        result.emplace_back(groupby_index_, i);
    }
    for(SizeT i = 0; i < aggregates_count; ++ i) {
        result.emplace_back(aggregate_index_, i);
    }
    return result;
}

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