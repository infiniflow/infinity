module;

export module logical_check;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import data_type;
import internal_types;
import check_statement;

namespace infinity {

export String ToString(CheckStmtType type);

export class LogicalCheck : public LogicalNode {
public:
    explicit LogicalCheck(u64 node_id, CheckStmtType type) : LogicalNode(node_id, LogicalNodeType::kCheck), check_type_(type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCheck"; }

    [[nodiscard]] CheckStmtType check_type() const { return check_type_; }

    // [[nodiscard]] inline u64 table_index() const { return table_index_; }
    //
    // [[nodiscard]] inline const String &schema_name() const { return schema_name_; }
    //
    // [[nodiscard]] inline const Optional<String> object_name() const { return object_name_; }
    //
    // [[nodiscard]] inline const Optional<u64> session_id() const { return session_id_; }
    //
    // [[nodiscard]] inline const Optional<String> file_path() const { return file_path_; }
    //
    // [[nodiscard]] inline const Optional<SegmentID> segment_id() const { return segment_id_; }
    //
    // [[nodiscard]] inline const Optional<TransactionID> transaction_id() const { return txn_id_; }
    //
    // [[nodiscard]] inline const Optional<BlockID> block_id() const { return block_id_; }
    //
    // [[nodiscard]] inline const Optional<ChunkID> chunk_id() const { return chunk_id_; }
    //
    // [[nodiscard]] inline const Optional<ColumnID> column_id() const { return column_id_; }
    //
    // [[nodiscard]] inline const Optional<String> index_name() const { return index_name_; }
    //
    // [[nodiscard]] inline const Optional<String> function_name() const { return function_name_; }

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};
    // String schema_name_;
    // Optional<String> object_name_; // It could be table/collection/view name
    // u64 table_index_{};
    //
    // Optional<String> file_path_;
    // Optional<SegmentID> segment_id_{};
    // Optional<BlockID> block_id_{};
    // Optional<ChunkID> chunk_id_{};
    // Optional<ColumnID> column_id_{};
    // Optional<String> index_name_{};
    // Optional<u64> session_id_{};
    // Optional<TransactionID> txn_id_{};
    // Optional<String> function_name_{};
};

} // namespace infinity
