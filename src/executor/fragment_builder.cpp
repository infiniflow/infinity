//
// Created by jinhai on 23-4-27.
//

module;

import stl;
import plan_fragment;
import physical_operator;
import physical_operator_type;
import physical_sink;
import physical_source;
import physical_explain;

import infinity_exception;
import parser;
import explain_fragment;
import fragment_context;
import logger;
import third_party;

module fragment_builder;

namespace infinity {

UniquePtr<PlanFragment> FragmentBuilder::BuildFragment(PhysicalOperator *phys_op) {
    auto plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
    plan_fragment->SetSinkNode(query_context_ptr_, SinkType::kResult, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
    BuildFragments(phys_op, plan_fragment.get());
    if (plan_fragment->GetSourceNode() != nullptr)
        plan_fragment->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
    return plan_fragment;
}

void FragmentBuilder::BuildExplain(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr) {

    PhysicalExplain *explain_op = (PhysicalExplain *)phys_op;
    switch (explain_op->explain_type()) {

        case ExplainType::kAnalyze: {
            Error<NotImplementException>("Not implement: Query analyze");
        }
        case ExplainType::kAst:
        case ExplainType::kUnOpt:
        case ExplainType::kOpt:
        case ExplainType::kPhysical: {
            current_fragment_ptr->AddOperator(phys_op);
            break;
        }
        case ExplainType::kFragment:
        case ExplainType::kPipeline: {
            // Build explain pipeline fragment
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            auto explain_child_fragment = this->BuildFragment(phys_op->left().get());

            // Generate explain context of the child fragment
            ExplainFragment::Explain(explain_child_fragment.get(), texts_ptr);

            explain_op->SetExplainText(texts_ptr);

            // Set texts to explain physical operator
            current_fragment_ptr->AddOperator(phys_op);

            if (explain_op->explain_type() == ExplainType::kPipeline) {
                current_fragment_ptr->AddChild(Move(explain_child_fragment));
            }
            break;
        }
    }
}

void FragmentBuilder::BuildFragments(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr) {
    switch (phys_op->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            Error<PlannerException>("Invalid physical operator type\"");
        }
        case PhysicalOperatorType::kExplain: {
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            BuildExplain(phys_op, current_fragment_ptr);
            return;
        }
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateIndex:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
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
            if (phys_op->left().get() != nullptr or phys_op->right().get() != nullptr) {
                Error<SchedulerException>(Format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kAggregate: {
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            if (phys_op->left().get() == nullptr) {
                Error<SchedulerException>("No input node of aggregate operator");
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left().get(), next_plan_fragment.get());
            current_fragment_ptr->AddChild(Move(next_plan_fragment));
            return;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kDelete: {
            if (phys_op->left().get() == nullptr) {
                Error<SchedulerException>(Format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left().get(), current_fragment_ptr);
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn: {
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            if (phys_op->left().get() == nullptr) {
                Error<SchedulerException>(Format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);

            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left().get(), next_plan_fragment.get());
            current_fragment_ptr->AddChild(Move(next_plan_fragment));
            return;
        }
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct: {
            Error<SchedulerException>(Format("Not support {}.", phys_op->GetName()));
        }
        case PhysicalOperatorType::kKnnScan: {
            //            current_fragment_ptr->AddSourceNode(query_context_ptr_,
            //                                                SourceType::kLocalQueue,
            //                                                phys_op->GetOutputNames(),
            //                                                phys_op->GetOutputTypes());
            //            if(phys_op->left() == nullptr) {
            //                ExecutorError(fmt::format("No input node of {}", phys_op->GetName()));
            //            }
            //            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            //            next_plan_fragment->AddSinkNode(query_context_ptr_,
            //                                            SinkType::kLocalQueue,
            //                                            phys_op->left()->GetOutputNames(),
            //                                            phys_op->left()->GetOutputTypes());
            //            BuildFragments(phys_op->left().get(), next_plan_fragment.get());
            //            current_fragment_ptr->AddChild(Move(next_plan_fragment));
            //            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            if (phys_op->left().get() != nullptr or phys_op->right().get() != nullptr) {
                Error<SchedulerException>(Format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kTable, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kIndexScan: {
            if (phys_op->left().get() != nullptr or phys_op->right().get() != nullptr) {
                Error<SchedulerException>(Format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kTable, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }

        case PhysicalOperatorType::kProjection: {
            current_fragment_ptr->AddOperator(phys_op);
            if (phys_op->left().get() == nullptr) {
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
                current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());

            } else {
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
                BuildFragments(phys_op->left().get(), current_fragment_ptr);
            }
            return;
        }
        default: {
            LOG_ERROR(Format("Invalid operator type: {} in Fragment Builder", phys_op->GetName()));
            break;
        }
    }
}

} // namespace infinity
