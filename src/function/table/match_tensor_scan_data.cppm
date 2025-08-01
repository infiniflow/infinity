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

export module infinity_core:match_tensor_scan_function_data;

import :stl;
import :function_data;
import :table_function;
import :global_block_id;
import internal_types;
import :knn_result_handler;
import :infinity_exception;
import :logger;
import :merge_knn;

namespace infinity {

export class MatchTensorScanFunctionData final : public TableFunctionData {
public:
    using TensorResultHandler = MergeKnnResultHandler<float>;

    explicit MatchTensorScanFunctionData(const u32 topn, const Optional<f32> knn_threshold) : topn_(topn), knn_threshold_(knn_threshold) { Init(); }

    void Init() {
        score_result_ = MakeUniqueForOverwrite<float[]>(topn_);
        row_id_result_ = MakeUniqueForOverwrite<RowID[]>(topn_);
        result_handler_ =
            GetMergeKnnResultHandler<ReservoirResultHandler, CompareMin, float>(1, topn_, score_result_.get(), row_id_result_.get(), knn_threshold_);
        result_handler_->Begin();
    }

    u32 End() {
        if (finished_) {
            UnrecoverableError("End() is called twice!");
            return 0;
        }
        finished_ = true;
        result_handler_->End();
        return result_handler_->GetSize(0); // WARNING: This relies on ReservoirResultHandler
    }

    bool finished_ = false;
    const u32 topn_ = 0;
    const Optional<f32> knn_threshold_;
    UniquePtr<float[]> score_result_;
    UniquePtr<RowID[]> row_id_result_;
    UniquePtr<TensorResultHandler> result_handler_;
};

} // namespace infinity
