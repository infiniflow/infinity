//
// Created by jinhai on 23-4-27.
//

#include "fragment_builder.h"
#include "explain_fragment.h"
#include "executor/operator/physical_explain.h"

namespace infinity {

UniquePtr<PlanFragment>
FragmentBuilder::BuildFragment(PhysicalOperator* phys_op) {
    auto plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
    plan_fragment->AddSinkNode(query_context_ptr_,
                               SinkType::kResult,
                               phys_op->GetOutputNames(),
                               phys_op->GetOutputTypes());
    BuildFragments(phys_op, plan_fragment.get());
    plan_fragment->AddSourceNode(query_context_ptr_,
                                 SourceType::kEmpty,
                                 phys_op->GetOutputNames(),
                                 phys_op->GetOutputTypes());
    return plan_fragment;
}

void
FragmentBuilder::BuildExplain(PhysicalOperator* phys_op, PlanFragment* current_fragment_ptr) {

    PhysicalExplain* explain_op = (PhysicalExplain*)phys_op;
    switch(explain_op->explain_type()) {

        case ExplainType::kAnalyze: {
            NotImplementError("Not implement: Query analyze");
        }
        case ExplainType::kAst:
        case ExplainType::kUnOpt:
        case ExplainType::kOpt:
        case ExplainType::kPhysical: {
            current_fragment_ptr->AddOperator(phys_op);
            break;
        }
        case ExplainType::kPipeline:{
            // Build explain pipeline fragment
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            auto explain_child_fragment = this->BuildFragment(phys_op->left().get());

            // Generate explain context of the child fragment
            ExplainFragment::Explain(explain_child_fragment.get(), texts_ptr);

            explain_op->SetExplainText(texts_ptr);

            // Set texts to explain physical operator
            current_fragment_ptr->AddOperator(phys_op);
            break;
        }
    }
}

void
FragmentBuilder::BuildFragments(PhysicalOperator* phys_op, PlanFragment *current_fragment_ptr) {
    switch (phys_op->operator_type()){
        case PhysicalOperatorType::kInvalid: {
            PlannerError("Invalid physical operator type")
        }
        case PhysicalOperatorType::kExplain: {
            LOG_INFO("Fragment Builder: Explain");
            BuildExplain(phys_op, current_fragment_ptr);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            return ;
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
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
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
            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->AddSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left().get(), next_plan_fragment.get());
            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
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
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
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
            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->AddSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left().get(), next_plan_fragment.get());
            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
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
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
            return ;
        }

        case PhysicalOperatorType::kProjection: {
            current_fragment_ptr->AddOperator(phys_op);
            if(phys_op->left() == nullptr) {
                current_fragment_ptr->AddSourceNode(query_context_ptr_,
                                                    SourceType::kEmpty,
                                                    phys_op->GetOutputNames(),
                                                    phys_op->GetOutputTypes());
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
            } else {
                BuildFragments(phys_op->left().get(), current_fragment_ptr);
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
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
