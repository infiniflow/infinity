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

export module match_tensor_scan_function_data;

import stl;
import function_data;
import table_function;
import global_block_id;
import block_index;
import internal_types;
import knn_result_handler;
import infinity_exception;
import logger;

namespace infinity {

export class MatchTensorScanFunctionData : public TableFunctionData {
public:
    using ResultHandler = ReservoirResultHandler<CompareMin<float, RowID>>;
    MatchTensorScanFunctionData(const BlockIndex *block_index, const SharedPtr<Vector<GlobalBlockID>> &global_block_ids, const u32 topn)
        : block_index_(block_index), global_block_ids_(global_block_ids), topn_(topn) {
        Init();
    }

    void Init() {
        score_result_ = MakeUniqueForOverwrite<float[]>(topn_);
        row_id_result_ = MakeUniqueForOverwrite<RowID[]>(topn_);
        result_handler_ = MakeUnique<ResultHandler>(1, topn_, score_result_.get(), row_id_result_.get());
        result_handler_->Begin();
    }

    u32 End() {
        if (finished_) {
            String error_message = "End() is called twice!";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            return 0;
        }
        finished_ = true;
        result_handler_->End();
        return result_handler_->GetSize(0);
    }

    bool finished_ = false;
    const BlockIndex *block_index_;
    const SharedPtr<Vector<GlobalBlockID>> &global_block_ids_;
    const u32 topn_;
    UniquePtr<float[]> score_result_;
    UniquePtr<RowID[]> row_id_result_;
    UniquePtr<ResultHandler> result_handler_;

    u32 current_block_ids_idx_ = 0;
};

} // namespace infinity
