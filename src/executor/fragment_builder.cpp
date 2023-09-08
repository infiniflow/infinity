//
// Created by jinhai on 23-4-27.
//

#include "fragment_builder.h"

namespace infinity {

SharedPtr<PlanFragment>
FragmentBuilder::Build(const SharedPtr<PhysicalOperator>& phys_op) const {
    switch(phys_op->operator_type()) {
        case PhysicalOperatorType::kAggregate: {
            return BuildAggregate(phys_op);
        }
        case PhysicalOperatorType::kUnionAll: {
            return BuildUnionAll(phys_op);
        }
        case PhysicalOperatorType::kTableScan: {
            return BuildTableScan(phys_op);
        }
        case PhysicalOperatorType::kFilter: {
            return BuildFilter(phys_op);
        }
        case PhysicalOperatorType::kIndexScan: {
            return BuildIndexScan(phys_op);
        }
        case PhysicalOperatorType::kDummyScan: {
            return BuildDummyScan(phys_op);
        }
        case PhysicalOperatorType::kJoinHash: {
            return BuildHashJoin(phys_op);
        }
        case PhysicalOperatorType::kJoinNestedLoop: {
            return BuildNestedLoopJoin(phys_op);
        }
        case PhysicalOperatorType::kJoinMerge: {
            return BuildMergeJoin(phys_op);
        }
        case PhysicalOperatorType::kJoinIndex: {
            return BuildIndexJoin(phys_op);
        }
        case PhysicalOperatorType::kCrossProduct: {
            return BuildCrossProduct(phys_op);
        }
        case PhysicalOperatorType::kLimit: {
            return BuildLimit(phys_op);
        }
        case PhysicalOperatorType::kTop: {
            return BuildTop(phys_op);
        }
        case PhysicalOperatorType::kProjection: {
            return BuildProject(phys_op);
        }
        case PhysicalOperatorType::kSort: {
            return BuildSort(phys_op);
        }
        case PhysicalOperatorType::kDelete: {
            return BuildDelete(phys_op);
        }
        case PhysicalOperatorType::kUpdate: {
            return BuildUpdate(phys_op);
        }
        case PhysicalOperatorType::kInsert: {
            return BuildInsert(phys_op);
        }
        case PhysicalOperatorType::kImport: {
            return BuildImport(phys_op);
        }
        case PhysicalOperatorType::kExport: {
            return BuildExport(phys_op);
        }
        case PhysicalOperatorType::kAlter: {
            return BuildAlter(phys_op);
        }
        case PhysicalOperatorType::kCreateTable: {
            return BuildCreateTable(phys_op);
        }
        case PhysicalOperatorType::kCreateCollection: {
            return BuildCreateCollection(phys_op);
        }
        case PhysicalOperatorType::kCreateSchema: {
            return BuildCreateSchema(phys_op);
        }
        case PhysicalOperatorType::kCreateView: {
            return BuildCreateView(phys_op);
        }
        case PhysicalOperatorType::kDropTable: {
            return BuildDropTable(phys_op);
        }
        case PhysicalOperatorType::kDropCollection: {
            return BuildDropCollection(phys_op);
        }
        case PhysicalOperatorType::kDropSchema: {
            return BuildDropSchema(phys_op);
        }
        case PhysicalOperatorType::kDropView: {
            return BuildDropView(phys_op);
        }
        case PhysicalOperatorType::kExplain: {
            return BuildExplain(phys_op);
        }
        case PhysicalOperatorType::kPreparedPlan: {
            return BuildPreparePlan(phys_op);
        }
        case PhysicalOperatorType::kShow: {
            return BuildShow(phys_op);
        }
        default: {
            PlannerError("Invalid physical operator type: ")
        }
    }
    NotImplementError("Not implemented")
    return nullptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildAggregate(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("Aggregate");
}


SharedPtr<PlanFragment>
FragmentBuilder::BuildTableScan(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    GeneralAssert(phys_op->IsExchange(), "TableScan operator is source");
    fragment_ptr->AddSourceNode(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildUnionAll(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("UnionAll");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildFilter(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() != nullptr, "Null left child in filter operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in filter operator");
    SharedPtr<PlanFragment> fragment_ptr = Build(phys_op->left());
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildIndexScan(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDummyScan(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildHashJoin(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("HashJoin");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildNestedLoopJoin(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("NestedLoopJoin");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildMergeJoin(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("MergeJoin");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildIndexJoin(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("IndexJoin");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildCrossProduct(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("CrossProduct");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildLimit(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("Limit");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildTop(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("Top");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildProject(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() != nullptr, "Null left child in project operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in project operator");
    SharedPtr<PlanFragment> fragment_ptr = Build(phys_op->left());
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildSort(const SharedPtr<PhysicalOperator>& phys_op) const {
//    PlannerAssert(phys_op->left() != nullptr, "Null left child in sort operator");
//    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in sort operator");
//    SharedPtr<PlanFragment> left_fragment_ptr = Build(phys_op->left());
//    SharedPtr<PhysicalSink> last_sink_node = MakeShared<PhysicalSink>(query_context_ptr_->GetNextNodeID());
//
//    SharedPtr<PhysicalExchange> last_exchange_node = MakeShared<PhysicalExchange>(query_context_ptr_->GetNextNodeID());
//
//
//
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);

    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDelete(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildUpdate(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildInsert(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildImport(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildExport(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildAlter(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildCreateTable(const SharedPtr<PhysicalOperator>& phys_op) const {
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    if(phys_op->left() != nullptr) {
        SharedPtr<PlanFragment> left_fragment_ptr = Build(phys_op);
        fragment_ptr->AddLeft(left_fragment_ptr);
    }
    fragment_ptr->AddOperator(phys_op);

    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildCreateCollection(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildCreateSchema(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildCreateView(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDropTable(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDropCollection(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDropSchema(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildDropView(const SharedPtr<PhysicalOperator>& phys_op) const {
    PlannerAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    PlannerAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildExplain(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("Explain");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildPreparePlan(const SharedPtr<PhysicalOperator>& phys_op) const {
    NotImplementError("Prepare");
}

SharedPtr<PlanFragment>
FragmentBuilder::BuildShow(const SharedPtr<PhysicalOperator>& phys_op) const {
    ExecutorAssert(phys_op->left() == nullptr, "Null left child in create collection operator");
    ExecutorAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");
    SharedPtr<PlanFragment> fragment_ptr = MakeShared<PlanFragment>();
    fragment_ptr->AddOperator(phys_op);
    return fragment_ptr;
}

void
FragmentBuilder::BuildFragments(const SharedPtr<PhysicalOperator> &phys_op, PlanFragment *current) {
    if (phys_op->IsSink()) {
        /// Build sink node

        SharedPtr<PhysicalOperator> fragment_child = nullptr;

        switch (phys_op->operator_type()) {
            case PhysicalOperatorType::kInsert:
            case PhysicalOperatorType::kDelete:
            case PhysicalOperatorType::kUpdate:
            case PhysicalOperatorType::kCreateTable:
            case PhysicalOperatorType::kCreateCollection:
            case PhysicalOperatorType::kCreateSchema:
            case PhysicalOperatorType::kTop:
            case PhysicalOperatorType::kLimit:
                ExecutorAssert(phys_op->left() != nullptr, "Null left child in sink operator");
                ExecutorAssert(phys_op->right() == nullptr, "Non-Null right child in sink operator");
                current->AddSourceNode(phys_op);
                fragment_child = phys_op->left();
                break;

            case PhysicalOperatorType::kJoinHash:
            case PhysicalOperatorType::kJoinNestedLoop:
                fragment_child = phys_op->right();
                current->AddOperator(phys_op);
                BuildFragments(phys_op->left(), current);
                break;

            default:
                throw Exception("Invalid sink operator type");
        }

        /// the current is dependent on this fragment to complete
        auto fragment = MakeShared<PlanFragment>();

        fragment->AddSinkNode(phys_op);
        current->AddDependency(fragment.get());

        ExecutorAssert(fragment_child != nullptr, "Null child in sink operator");

        BuildFragments(fragment_child, fragment.get());
    }else{
        /// operator is not sink
        switch (phys_op->operator_type()){
            ExecutorAssert(phys_op->left() == nullptr, "Non-Null left child in create collection operator");
            ExecutorAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");

            case PhysicalOperatorType::kUnionAll:
            /// do something

            default:
                break;
        }
        /// physical opt child is empty
        if (phys_op->left() == nullptr&& phys_op->right() == nullptr) {
            current->AddSourceNode(phys_op);
        }else{
            current->AddOperator(phys_op);
            BuildFragments(phys_op->left(), current);
        }
    }
}
}
