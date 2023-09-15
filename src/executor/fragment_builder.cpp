//
// Created by jinhai on 23-4-27.
//

#include "fragment_builder.h"
#include "explain_fragment.h"
#include "executor/operator/physical_explain.h"

namespace infinity {

UniquePtr<PlanFragment>
FragmentBuilder::BuildFragment(PhysicalOperator* phys_op) {
    auto plan_fragment = MakeUnique<PlanFragment>(query_context_ptr_, SinkType::kResult, phys_op);
    BuildFragments(phys_op, plan_fragment.get());
    return plan_fragment;
}

void
FragmentBuilder::BuildExplain(PhysicalOperator* phys_op, PlanFragment* current_fragment_ptr) {

    // Build explain pipeline fragment
    SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
    auto explain_child_fragment = this->BuildFragment(phys_op->left().get());

    // Generate explain context of the child fragment
    ExplainFragment::Explain(explain_child_fragment.get(), texts_ptr);
    PhysicalExplain* explain_op = (PhysicalExplain*)phys_op;
    explain_op->SetExplainText(texts_ptr);

    // Set texts to explain physical operator
    current_fragment_ptr->AddOperator(phys_op);
}

void
FragmentBuilder::BuildFragments(PhysicalOperator* phys_op, PlanFragment *current_fragment_ptr) {
    switch (phys_op->operator_type()){
        case PhysicalOperatorType::kInvalid: {
            PlannerError("Invalid physical operator type")
        }
        case PhysicalOperatorType::kExplain: {
            LOG_INFO("Fragment Builder: Explain");
            return BuildExplain(phys_op, current_fragment_ptr);
        }
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kShow:
        case PhysicalOperatorType::kFlush:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport: {
            current_fragment_ptr->AddOperator(phys_op);
            if(phys_op->left() != nullptr or phys_op->right() != nullptr) {
                SchedulerError(fmt::format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                SourceType::kEmpty,
                                                phys_op->GetOutputNames(),
                                                phys_op->GetOutputTypes());
            return ;
        }
        case PhysicalOperatorType::kAggregate: {
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                SourceType::kLocalQueue,
                                                phys_op->GetOutputNames(),
                                                phys_op->GetOutputTypes());
            if(phys_op->left() == nullptr) {
                SchedulerError("No input node of aggregate operator")
            }
            auto next_plan_fragment = MakeUnique<PlanFragment>(query_context_ptr_,
                                                               SinkType::kLocalQueue,
                                                               phys_op->left().get());
            BuildFragments(phys_op, next_plan_fragment.get());
            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            return ;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kDelete: {
            if(phys_op->left() == nullptr) {
                SchedulerError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left().get(), current_fragment_ptr);
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort: {
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                SourceType::kLocalQueue,
                                                phys_op->GetOutputNames(),
                                                phys_op->GetOutputTypes());
            if(phys_op->left() == nullptr) {
                SchedulerError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            auto next_plan_fragment = MakeUnique<PlanFragment>(query_context_ptr_,
                                                               SinkType::kLocalQueue,
                                                               phys_op->left().get());
            BuildFragments(phys_op, next_plan_fragment.get());
            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            return ;
        }
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kUpdate: {
            SchedulerError(fmt::format("Not support {}.", phys_op->GetName()));
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kIndexScan: {
            if (phys_op->left() != nullptr or phys_op->right() != nullptr) {
                SchedulerError(fmt::format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                SourceType::kTable,
                                                phys_op->GetOutputNames(),
                                                phys_op->GetOutputTypes());
            return ;
        }

        case PhysicalOperatorType::kProjection: {
            current_fragment_ptr->AddOperator(phys_op);
            if(phys_op->left() == nullptr) {
                current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                    SourceType::kEmpty,
                                                    phys_op->GetOutputNames(),
                                                    phys_op->GetOutputTypes());
            } else {
                BuildFragments(phys_op->left().get(), current_fragment_ptr);
            }
            return ;
        }
        default: {
            LOG_ERROR("Invalid operator type: {} in Fragment Builder", phys_op->GetName());
            break;
        }
    }
}

}
