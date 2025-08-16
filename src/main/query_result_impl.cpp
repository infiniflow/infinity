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

module infinity_core:query_result.impl;

import :query_result;
import :logical_node_type;
import :data_block;
import :data_table;

import std;

namespace infinity {

std::string QueryResult::ToString() const {
    std::stringstream ss;

    switch (root_operator_type_) {
        case LogicalNodeType::kInsert: {
            return "INSERT 0 1";
        }
        case LogicalNodeType::kUpdate: {
            return "UPDATE 0 1";
        }
        case LogicalNodeType::kDelete: {
            return "DELETE 0 1";
        }
        default: {
            ss << std::endl;
        }
    }

    size_t column_count = result_table_->ColumnCount();
    for (size_t idx = 0; idx < column_count; ++idx) {
        std::string end;
        if (idx != column_count - 1) {
            end = " ";
        }
        ss << result_table_->GetColumnNameById(idx) << end;
    }
    ss << std::endl;

    // Get Block count
    size_t block_count = result_table_->DataBlockCount();

    // Iterate all blocks
    for (size_t idx = 0; idx < block_count; ++idx) {
        // Get current block
        DataBlock *current_block = result_table_->GetDataBlockById(idx).get();
        ss << current_block->ToString();
    }

    return ss.str();
}

} // namespace infinity