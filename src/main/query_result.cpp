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

#include <sstream>

import stl;
import logical_node_type;
import data_block;

module query_result;

namespace infinity {

String QueryResult::ToString() const {
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

    SizeT column_count = result_table_->ColumnCount();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        String end;
        if (idx != column_count - 1) {
            end = " ";
        }
        ss << result_table_->GetColumnNameById(idx) << end;
    }
    ss << std::endl;

    // Get Block count
    SizeT block_count = result_table_->DataBlockCount();

    // Iterate all blocks
    for (SizeT idx = 0; idx < block_count; ++idx) {
        // Get current block
        DataBlock *current_block = result_table_->GetDataBlockById(idx).get();
        ss << current_block->ToString();
    }

    return ss.str();
}

} // namespace infinity