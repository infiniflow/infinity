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

export module infinity_core:logical_show;

import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import show_statement;

namespace infinity {

export std::string ToString(ShowStmtType type);

export class LogicalShow : public LogicalNode {
public:
    explicit LogicalShow(u64 node_id,
                         ShowStmtType type,
                         std::string schema_name,
                         std::optional<std::string> object_name,
                         u64 table_index,
                         std::optional<std::string> file_path = std::nullopt,
                         std::optional<SegmentID> segment_id = std::nullopt,
                         std::optional<BlockID> block_id = std::nullopt,
                         std::optional<ChunkID> chunk_id = std::nullopt,
                         std::optional<ColumnID> column_id = std::nullopt,
                         std::optional<std::string> index_name = std::nullopt,
                         std::optional<TransactionID> session_id = std::nullopt,
                         std::optional<u64> txn_id = std::nullopt,
                         std::optional<std::string> function_name = std::nullopt)
        : LogicalNode(node_id, LogicalNodeType::kShow), show_type_(type), schema_name_(std::move(schema_name)), object_name_(std::move(object_name)),
          table_index_(table_index), file_path_(std::move(file_path)), segment_id_(segment_id), block_id_(block_id), chunk_id_(chunk_id),
          column_id_(column_id), index_name_(index_name), session_id_(session_id), txn_id_(txn_id), function_name_(function_name) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalShow"; }

    [[nodiscard]] ShowStmtType show_type() const { return show_type_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const std::string &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const std::optional<std::string> object_name() const { return object_name_; }

    [[nodiscard]] inline const std::optional<u64> session_id() const { return session_id_; }

    [[nodiscard]] inline const std::optional<std::string> file_path() const { return file_path_; }

    [[nodiscard]] inline const std::optional<SegmentID> segment_id() const { return segment_id_; }

    [[nodiscard]] inline const std::optional<TransactionID> transaction_id() const { return txn_id_; }

    [[nodiscard]] inline const std::optional<BlockID> block_id() const { return block_id_; }

    [[nodiscard]] inline const std::optional<ChunkID> chunk_id() const { return chunk_id_; }

    [[nodiscard]] inline const std::optional<ColumnID> column_id() const { return column_id_; }

    [[nodiscard]] inline const std::optional<std::string> index_name() const { return index_name_; }

    [[nodiscard]] inline const std::optional<std::string> function_name() const { return function_name_; }

private:
    ShowStmtType show_type_{ShowStmtType::kInvalid};
    std::string schema_name_;
    std::optional<std::string> object_name_; // It could be table/collection/view name
    u64 table_index_{};

    std::optional<std::string> file_path_;
    std::optional<SegmentID> segment_id_{};
    std::optional<BlockID> block_id_{};
    std::optional<ChunkID> chunk_id_{};
    std::optional<ColumnID> column_id_{};
    std::optional<std::string> index_name_{};
    std::optional<u64> session_id_{};
    std::optional<TransactionID> txn_id_{};
    std::optional<std::string> function_name_{};
};

} // namespace infinity
