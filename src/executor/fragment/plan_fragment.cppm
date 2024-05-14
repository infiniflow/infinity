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

export module plan_fragment;

import stl;

import data_table;
import fragment_context;
import physical_operator;
import physical_source;
import physical_sink;
import query_context;
import internal_types;
import data_type;

namespace infinity {

export class PlanFragment {
public:
    PlanFragment() = default;

    explicit inline PlanFragment(u64 fragment_id) : fragment_id_(fragment_id) {}

    virtual ~PlanFragment() = default;

    void SetFragmentType(FragmentType fragment_type) { fragment_type_ = fragment_type; }

    [[nodiscard]] inline FragmentType GetFragmentType() const { return fragment_type_; }

    inline void AddOperator(PhysicalOperator *op) { operators_.emplace_back(op); }

    inline Vector<PhysicalOperator *> &GetOperators() { return operators_; }

    void SetSourceNode(QueryContext *query_context,
                       SourceType source_type,
                       const SharedPtr<Vector<String>> &names,
                       const SharedPtr<Vector<SharedPtr<DataType>>> &types);

    void SetSinkNode(QueryContext *query_context,
                     SinkType sink_type,
                     const SharedPtr<Vector<String>> &names,
                     const SharedPtr<Vector<SharedPtr<DataType>>> &types);

    [[nodiscard]] inline PhysicalSource *GetSourceNode() const { return source_.get(); }

    [[nodiscard]] inline PhysicalSink *GetSinkNode() const { return sink_.get(); }

    [[nodiscard]] inline PlanFragment *GetParent() const { return parent_; }

    inline void AddChild(UniquePtr<PlanFragment> child_fragment) {
        child_fragment->parent_ = this;
        children_.emplace_back(std::move(child_fragment));
    }

    inline Vector<UniquePtr<PlanFragment>> &Children() { return children_; }

    inline bool HasChild() { return !children_.empty(); }

    SharedPtr<Vector<String>> ToString();

    [[nodiscard]] inline u64 FragmentID() const { return fragment_id_; }

    inline void SetContext(UniquePtr<FragmentContext> context) { context_ = std::move(context); }

    inline FragmentContext *GetContext() { return context_.get(); }

    SharedPtr<DataTable> GetResult();

    void WaitForFinish() { context_->WaitForFinish(); }

private:
    u64 fragment_id_{};

    UniquePtr<PhysicalSink> sink_{};

    Vector<PhysicalOperator *> operators_{};

    UniquePtr<PhysicalSource> source_{};

    PlanFragment *parent_{};

    Vector<UniquePtr<PlanFragment>> children_{};

    UniquePtr<FragmentContext> context_{};

    FragmentType fragment_type_{FragmentType::kSerialMaterialize};
};

} // namespace infinity
