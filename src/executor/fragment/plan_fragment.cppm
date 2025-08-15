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

export module infinity_core:plan_fragment;

import :data_table;
import :fragment_context;
import :physical_operator;
import :physical_source;
import :physical_sink;
import :query_context;

import internal_types;
import data_type;
import global_resource_usage;

namespace infinity {

export class PlanFragment {
public:
    PlanFragment() = default;

    explicit inline PlanFragment(u64 fragment_id) : fragment_id_(fragment_id) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PlanFragment");
#endif
    }

    virtual ~PlanFragment() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PlanFragment");
#endif
    }

    void SetFragmentType(FragmentType fragment_type) { fragment_type_ = fragment_type; }

    [[nodiscard]] inline FragmentType GetFragmentType() const { return fragment_type_; }

    inline void AddOperator(PhysicalOperator *op) { operators_.emplace_back(op); }

    inline std::vector<PhysicalOperator *> &GetOperators() { return operators_; }

    void SetSourceNode(QueryContext *query_context,
                       SourceType source_type,
                       const std::shared_ptr<std::vector<std::string>> &names,
                       const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types);

    void SetSinkNode(QueryContext *query_context,
                     SinkType sink_type,
                     const std::shared_ptr<std::vector<std::string>> &names,
                     const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types);

    [[nodiscard]] inline PhysicalSource *GetSourceNode() const { return source_.get(); }

    [[nodiscard]] inline PhysicalSink *GetSinkNode() const { return sink_.get(); }

    [[nodiscard]] inline std::vector<PlanFragment *> GetParents() const { return parents_; }

    inline void AddChild(std::shared_ptr<PlanFragment> child_fragment) {
        child_fragment->parents_.emplace_back(this);
        children_.emplace_back(std::move(child_fragment));
    }

    inline std::vector<std::shared_ptr<PlanFragment>> &Children() { return children_; }

    inline bool HasChild() { return !children_.empty(); }

    std::shared_ptr<std::vector<std::string>> ToString();

    [[nodiscard]] inline u64 FragmentID() const { return fragment_id_; }

    inline void SetContext(std::unique_ptr<FragmentContext> context) { context_ = std::move(context); }

    inline FragmentContext *GetContext() { return context_.get(); }

    std::shared_ptr<DataTable> GetResult();

    static void AddNext(std::shared_ptr<PlanFragment> root, PlanFragment *next);

    size_t GetStartFragments(std::vector<PlanFragment *> &leaf_fragments);

private:
    u64 fragment_id_{};

    std::unique_ptr<PhysicalSink> sink_{};

    std::vector<PhysicalOperator *> operators_{};

    std::unique_ptr<PhysicalSource> source_{};

    std::vector<PlanFragment *> parents_{};

    std::vector<std::shared_ptr<PlanFragment>> children_{};

    std::unique_ptr<FragmentContext> context_{};

    FragmentType fragment_type_{FragmentType::kSerialMaterialize};
};

} // namespace infinity
