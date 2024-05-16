// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <vector>

module fragment_builder;

import stl;
import plan_fragment;
import physical_operator;
import physical_operator_type;
import physical_sink;
import physical_source;
import physical_explain;
import physical_knn_scan;
import status;
import infinity_exception;

import explain_fragment;
import fragment_context;
import logger;
import third_party;
import explain_statement;

namespace infinity {

SharedPtr<PlanFragment> FragmentBuilder::BuildFragment(const Vector<PhysicalOperator *> &phys_ops) {
    SharedPtr<PlanFragment> result = nullptr;
    for (auto *phys_op : phys_ops) {
        auto plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
        plan_fragment->SetSinkNode(query_context_ptr_, SinkType::kResult, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
        BuildFragments(phys_op, plan_fragment.get());
        if (plan_fragment->GetSourceNode() == nullptr) {
            plan_fragment->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
        }
        if (result.get() == nullptr) {
            result = std::move(plan_fragment);
        } else {
            PlanFragment::AddNext(result, plan_fragment.get());
            result = std::move(plan_fragment);
        }
    }
    return result;
}

void FragmentBuilder::BuildExplain(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr) {

    PhysicalExplain *explain_op = (PhysicalExplain *)phys_op;
    switch (explain_op->explain_type()) {

        case ExplainType::kAnalyze: {
            RecoverableError(Status::NotSupport("Not implement: Query analyze"));
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
            Vector<PhysicalOperator *> phys_ops{phys_op->left()};
            auto explain_child_fragment = this->BuildFragment(phys_ops);

            // Generate explain context of the child fragment
            ExplainFragment::Explain(explain_child_fragment.get(), texts_ptr);

            explain_op->SetExplainText(texts_ptr);

            // Set texts to explain physical operator
            current_fragment_ptr->AddOperator(phys_op);

            if (explain_op->explain_type() == ExplainType::kPipeline) {
                current_fragment_ptr->AddChild(std::move(explain_child_fragment));
            }
            break;
        }
        case ExplainType::kInvalid: {
            UnrecoverableError("Invalid explain type");
        }
    }
}

void FragmentBuilder::BuildFragments(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr) {
    switch (phys_op->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            UnrecoverableError("Invalid physical operator type");
        }
        case PhysicalOperatorType::kExplain: {
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            BuildExplain(phys_op, current_fragment_ptr);
            return;
        }
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
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
        case PhysicalOperatorType::kOptimize:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kMatch: {
            current_fragment_ptr->AddOperator(phys_op);
            if (phys_op->left() != nullptr or phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kAggregate: {
            current_fragment_ptr->AddOperator(phys_op);
            if (phys_op->left() == nullptr) {
                UnrecoverableError("No input node of aggregate operator");
            } else {
                BuildFragments(phys_op->left(), current_fragment_ptr);
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            }
            return;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit: {
            if (phys_op->left() == nullptr) {
                UnrecoverableError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left(), current_fragment_ptr);
            break;
        }
        case PhysicalOperatorType::kTop: {
            if (phys_op->left() == nullptr) {
                UnrecoverableError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left(), current_fragment_ptr);
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            break;
        }
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kSort: {
            if (phys_op->left() == nullptr) {
                UnrecoverableError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            BuildFragments(phys_op->left(), current_fragment_ptr);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            break;
        }
        case PhysicalOperatorType::kFusion:
        case PhysicalOperatorType::kMergeAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn: {
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            if (phys_op->left() == nullptr) {
                UnrecoverableError(fmt::format("No input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);

            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left(), next_plan_fragment.get());
            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            if (phys_op->right() != nullptr) {
                auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
                next_plan_fragment->SetSinkNode(query_context_ptr_,
                                                SinkType::kLocalQueue,
                                                phys_op->right()->GetOutputNames(),
                                                phys_op->right()->GetOutputTypes());
                BuildFragments(phys_op->right(), next_plan_fragment.get());
                current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            }
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
            UnrecoverableError(fmt::format("Not support {}.", phys_op->GetName()));
        }
        case PhysicalOperatorType::kKnnScan: {
            if (phys_op->left() != nullptr or phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("{} shouldn't have child.", phys_op->GetName()));
            }
            PhysicalKnnScan *knn_scan = static_cast<PhysicalKnnScan *>(phys_op);
            if (knn_scan->TaskCount() == 1) {
                current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            } else {
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kTable, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kIndexScan: {
            if (phys_op->left() != nullptr or phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("{} shouldn't have child.", phys_op->GetName()));
            }
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kTable, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }

        case PhysicalOperatorType::kProjection: {
            current_fragment_ptr->AddOperator(phys_op);
            if (phys_op->left() == nullptr) {
                // Project will be only operator
                current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
                current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());

            } else {
                current_fragment_ptr->SetFragmentType(FragmentType::kParallelStream);
                BuildFragments(phys_op->left(), current_fragment_ptr);
            }
            return;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            if (phys_op->left() != nullptr || phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kCreateIndexDo: {
            if (phys_op->left() == nullptr || phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());

            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left(), next_plan_fragment.get());

            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            return;
        }
        case PhysicalOperatorType::kCreateIndexFinish: {
            if (phys_op->left() == nullptr || phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());

            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left(), next_plan_fragment.get());

            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            return;
        }
        case PhysicalOperatorType::kCompact: {
            if (phys_op->left() != nullptr || phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            return;
        }
        case PhysicalOperatorType::kCompactIndexPrepare: {
            if (phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kEmpty, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            if (phys_op->left() != nullptr) {
                auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
                next_plan_fragment->SetSinkNode(query_context_ptr_,
                                                SinkType::kLocalQueue,
                                                phys_op->left()->GetOutputNames(),
                                                phys_op->left()->GetOutputTypes());
                BuildFragments(phys_op->left(), next_plan_fragment.get());
                current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            }
            if (phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            return;
        }
        case PhysicalOperatorType::kCompactIndexDo: {
            if (phys_op->left() == nullptr || phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kParallelMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());

            auto next_plan_fragment = MakeUnique<PlanFragment>(GetFragmentId());
            next_plan_fragment->SetSinkNode(query_context_ptr_,
                                            SinkType::kLocalQueue,
                                            phys_op->left()->GetOutputNames(),
                                            phys_op->left()->GetOutputTypes());
            BuildFragments(phys_op->left(), next_plan_fragment.get());

            current_fragment_ptr->AddChild(std::move(next_plan_fragment));
            return;
        }
        case PhysicalOperatorType::kCompactFinish: {
            if (phys_op->left() == nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            current_fragment_ptr->AddOperator(phys_op);
            current_fragment_ptr->SetFragmentType(FragmentType::kSerialMaterialize);
            current_fragment_ptr->SetSourceNode(query_context_ptr_, SourceType::kLocalQueue, phys_op->GetOutputNames(), phys_op->GetOutputTypes());
            if (phys_op->left() != nullptr) {
                auto next_plan_fragment1 = MakeUnique<PlanFragment>(GetFragmentId());
                next_plan_fragment1->SetSinkNode(query_context_ptr_,
                                                 SinkType::kLocalQueue,
                                                 phys_op->left()->GetOutputNames(),
                                                 phys_op->left()->GetOutputTypes());
                BuildFragments(phys_op->left(), next_plan_fragment1.get());
                current_fragment_ptr->AddChild(std::move(next_plan_fragment1));
                if (phys_op->right() != nullptr) {
                    auto next_plan_fragment2 = MakeUnique<PlanFragment>(GetFragmentId());
                    next_plan_fragment2->SetSinkNode(query_context_ptr_,
                                                     SinkType::kLocalQueue,
                                                     phys_op->right()->GetOutputNames(),
                                                     phys_op->right()->GetOutputTypes());
                    BuildFragments(phys_op->right(), next_plan_fragment2.get());
                    current_fragment_ptr->AddChild(std::move(next_plan_fragment2));
                }
            } else if (phys_op->right() != nullptr) {
                UnrecoverableError(fmt::format("Invalid input node of {}", phys_op->GetName()));
            }
            return;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid operator type: {} in Fragment Builder", phys_op->GetName()));
        }
    }
}

} // namespace infinity
