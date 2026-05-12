// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:column_inverter_json_array;

import :column_inverter;
import :posting_writer;
import :json_manager;
import :vector_with_lock;
import :column_vector;

namespace infinity {

export class ColumnInverterJsonArray : public ColumnInverter {
public:
    ~ColumnInverterJsonArray() = default;

    ColumnInverterJsonArray(PostingWriterProvider posting_writer_provider, VectorWithLock<u32> &column_lengths, const std::string &json_path);

    void InitJsonPath(const std::string &json_path);

    size_t InvertColumn(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count, u32 begin_doc_id);

private:
    std::string json_path_{};
    std::vector<JsonTokenInfo> json_tokens_{};
};

} // namespace infinity