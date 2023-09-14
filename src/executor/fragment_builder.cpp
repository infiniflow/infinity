//
// Created by jinhai on 23-4-27.
//

#include "fragment_builder.h"
#include "explain_fragment.h"
#include "executor/operator/physical_explain.h"

namespace infinity {

void
FragmentBuilder::BuildExplain(PhysicalOperator* phys_op, PlanFragment* current_fragment_ptr) const {

    // Build explain pipeline fragment
    SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
    auto explain_child_fragment = MakeShared<PlanFragment>();
    BuildFragments(phys_op->left().get(), explain_child_fragment.get());

    // Generate explain context of the child fragment
    ExplainFragment::Explain(explain_child_fragment.get(), texts_ptr);
    PhysicalExplain* explain_op = (PhysicalExplain*)phys_op;
    explain_op->SetExplainText(texts_ptr);

    // Set texts to explain physical operator
    current_fragment_ptr->AddOperator(phys_op);
}

void
FragmentBuilder::BuildFragments(PhysicalOperator* phys_op, PlanFragment *current_fragment_ptr) const {
    if (phys_op->IsSink()) {
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
                current_fragment_ptr->SetSourceNode(phys_op);
                fragment_child = phys_op->left();
                break;

            case PhysicalOperatorType::kJoinHash:
            case PhysicalOperatorType::kJoinNestedLoop:
                fragment_child = phys_op->right();
                current_fragment_ptr->AddOperator(phys_op);
                BuildFragments(phys_op->left().get(), current_fragment_ptr);
                break;

            default:
                PlannerError("Invalid operator type")
        }

        /// the current is dependent on this fragment to complete
        auto fragment = MakeShared<PlanFragment>();

        fragment->SetSinkNode(phys_op);
        current_fragment_ptr->AddDependency(fragment.get());

        ExecutorAssert(fragment_child != nullptr, "Null child in sink operator");

        BuildFragments(fragment_child.get(), fragment.get());
    } else {
        switch (phys_op->operator_type()){
            case PhysicalOperatorType::kExplain: {
                LOG_INFO("Fragment Builder: Explain");
                return BuildExplain(phys_op, current_fragment_ptr);
            }
            case PhysicalOperatorType::kAlter:
            case PhysicalOperatorType::kCreateTable:
            case PhysicalOperatorType::kCreateCollection:
            case PhysicalOperatorType::kCreateSchema:
            case PhysicalOperatorType::kCreateView:
            case PhysicalOperatorType::kDropTable:
            case PhysicalOperatorType::kDropCollection:
            case PhysicalOperatorType::kDropSchema:
            case PhysicalOperatorType::kDropView:
            case PhysicalOperatorType::kPreparedPlan:
            case PhysicalOperatorType::kShow:
            case PhysicalOperatorType::kImport:
            case PhysicalOperatorType::kFlush: {
                current_fragment_ptr->AddOperator(phys_op);
                LOG_INFO("Fragment Builder: {}", phys_op->GetName());
                return ;
            }
//            ExecutorAssert(phys_op->left() == nullptr, "Non-Null left child in create collection operator");
//            ExecutorAssert(phys_op->right() == nullptr, "Non-Null right child in create collection operator");


            default: {
                LOG_INFO("Default Fragment Builder: {}", phys_op->GetName());
                break;
            }
        }

        if (phys_op->left() == nullptr&& phys_op->right() == nullptr) {
            current_fragment_ptr->SetSourceNode(phys_op);
        }else{
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left().get(), current_fragment_ptr);
        }
    }
}

}
