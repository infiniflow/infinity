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

export module infinity_core:physical_operator;

import :stl;
import :physical_operator_type;
import :base_table_ref;
import data_type;

namespace infinity {
class DataTable;
class OperatorState;
class QueryContext;
struct LoadMeta;

export class PhysicalOperator : public EnableSharedFromThis<PhysicalOperator> {

public:
    inline explicit PhysicalOperator(const PhysicalOperatorType type,
                                     UniquePtr<PhysicalOperator> left,
                                     UniquePtr<PhysicalOperator> right,
                                     const u64 id,
                                     SharedPtr<Vector<LoadMeta>> load_metas,
                                     bool cache_result = false)
        : operator_id_(id), operator_type_(type), left_(std::move(left)), right_(std::move(right)), load_metas_(std::move(load_metas)),
          cache_result_(cache_result) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PhysicalOperator");
#endif
    }

    virtual ~PhysicalOperator() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PhysicalOperator");
#endif
    }

    virtual void Init(QueryContext* query_context) = 0;

    virtual SizeT TaskletCount();

    inline PhysicalOperator *left() const { return left_.get(); }

    inline PhysicalOperator *right() const { return right_.get(); }

    u64 node_id() const { return operator_id_; }

    /// for push based execution. returns if done some real work. It determins whether FragmentTask schedule remaining operators.
    virtual bool Execute(QueryContext *query_context, OperatorState *output_state) = 0;

    const SharedPtr<DataTable> &output() const { return output_; }

    inline PhysicalOperatorType operator_type() const { return operator_type_; }

    virtual SharedPtr<Vector<String>> GetOutputNames() const = 0;

    virtual SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const = 0;

    virtual String GetName() const;

    void InputLoad(QueryContext *query_context, OperatorState *output_state, HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs);

    virtual void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) {}

    const SharedPtr<Vector<LoadMeta>> &load_metas() const { return load_metas_; }

    bool cache_result() const { return cache_result_; }

    void SetCacheResult(bool cache_result) { cache_result_ = cache_result; }

protected:
    u64 operator_id_;
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    UniquePtr<PhysicalOperator> left_{nullptr};
    UniquePtr<PhysicalOperator> right_{nullptr};

    SharedPtr<DataTable> output_{};

    SharedPtr<Vector<LoadMeta>> load_metas_{};
    bool cache_result_;

public:
    // Operator

    virtual bool ParallelOperator() const { return false; }

public:
    // Exchange
    virtual bool IsExchange() const { return false; }

    virtual bool ParallelExchange() const { return false; }

public:
    // Sink
    virtual bool IsSink() const { return false; }

    virtual bool ParallelSink() const { return false; }

    virtual bool SinkOrderMatters() const { return false; }
};

// three common implementations for physical operator member function when load_metas_ is applied
// ref: src/executor/physical_operator.cpp:35
export struct PhysicalCommonFunctionUsingLoadMeta {
    static SharedPtr<Vector<String>> GetOutputNames(const PhysicalOperator &op);
    static SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes(const PhysicalOperator &op);
};

struct OutputJobInfo {
    // src data info
    SegmentID segment_id_{};
    BlockID block_id_{};
    ColumnID column_id_{};
    BlockOffset block_offset_{};
    // target position
    u32 output_block_id_{};
    u32 output_column_id_{};
    u32 output_row_id_{};
    friend auto operator<=>(const OutputJobInfo &, const OutputJobInfo &) = default;
};

class BufferManager;
struct BlockIndex;
struct DataBlock;
export struct OutputToDataBlockHelper {
    Vector<OutputJobInfo> output_job_infos;
    void AddOutputJobInfo(const SegmentID segment_id,
                          const BlockID block_id,
                          const ColumnID column_id,
                          const BlockOffset block_offset,
                          const u32 output_block_id,
                          const u32 output_column_id,
                          const u32 output_row_id) {
        output_job_infos.emplace_back(segment_id, block_id, column_id, block_offset, output_block_id, output_column_id, output_row_id);
    }
    void OutputToDataBlock(BufferManager *buffer_mgr, const BlockIndex *block_index, const Vector<UniquePtr<DataBlock>> &output_data_blocks);
};

} // namespace infinity
