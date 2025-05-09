module;

export module physical_check;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import logical_show;
import load_meta;
import infinity_exception;
import internal_types;
import column_def;
import data_type;
import variables;
import data_block;
import logger;
import check_statement;

namespace infinity {

export class PhysicalCheck : public PhysicalOperator {
public:
    explicit PhysicalCheck(u64 id, CheckStmtType type, Optional<SizeT> tag_id, SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCheck, nullptr, nullptr, id, load_metas), check_type_(type), tag_id_(tag_id) {}

    ~PhysicalCheck() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline CheckStmtType check_type() const { return check_type_; }

    // inline const String &db_name() const { return db_name_; };
    //
    // inline const Optional<String> object_name() const { return object_name_; };

private:
    // void ExecuteShowViewDetail(QueryContext *query_context,
    //                            const SharedPtr<Vector<SharedPtr<DataType>>> &column_types,
    //                            const SharedPtr<Vector<String>> &column_names);

    void ExecuteCheckSystem(QueryContext *query_context, CheckOperatorState *operator_state);

    void ExecuteCheckTable(QueryContext *query_context, CheckOperatorState *operator_state);

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};
    Optional<SizeT> tag_id_;
    // String db_name_{};
    // Optional<String> object_name_{};
    // u64 table_index_{};
    //
    // Optional<String> file_path_{};
    // Optional<SegmentID> segment_id_{};
    // Optional<BlockID> block_id_{};
    // Optional<ChunkID> chunk_id_{};
    // Optional<ColumnID> column_id_{};
    // Optional<String> index_name_{};
    // Optional<u64> session_id_{};
    // Optional<TransactionID> txn_id_{};
    // Optional<String> function_name_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
