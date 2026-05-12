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

module;

#include <cstdio>

module infinity_core:column_inverter_json_array.impl;

import :column_inverter_json_array;
import :column_inverter;
import :analyzer;
import :analyzer_pool;
import :string_ref;
import :term;
import :json_manager;
import :infinity_exception;
import :status;
import :logger;

import std.compat;

namespace infinity {

ColumnInverterJsonArray::ColumnInverterJsonArray(PostingWriterProvider posting_writer_provider,
                                                 VectorWithLock<u32> &column_lengths,
                                                 const std::string &json_path)
    : ColumnInverter(posting_writer_provider, column_lengths), json_path_(json_path) {
    InitJsonPath(json_path);
}

void ColumnInverterJsonArray::InitJsonPath(const std::string &json_path) {
    auto [valid, tokens] = JsonManager::get_json_tokens(json_path);
    if (!valid) {
        Status status = Status::InvalidIndexDefinition(fmt::format("Invalid JSON path: {}", json_path));
        RecoverableError(status);
    }
    json_tokens_ = std::move(tokens);
}

size_t ColumnInverterJsonArray::InvertColumn(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count, u32 begin_doc_id) {
    begin_doc_id_ = begin_doc_id;
    doc_count_ = row_count;
    std::vector<u32> column_lengths(row_count);
    size_t term_count_sum = 0;
    for (size_t i = 0; i < row_count; ++i) {
        std::string json_str = column_vector->ToString(row_offset + i);
        if (json_str.empty()) {
            continue;
        }

        auto json_obj = JsonManager::parse(json_str);
        auto [exist, extracted] = JsonManager::json_extract(json_obj, json_tokens_);
        if (!exist) {
            column_lengths[i] = 0;
            continue;
        }

        auto parsed_extracted = JsonManager::parse(extracted);
        auto [elem_count, elements] = JsonManager::json_unnest(parsed_extracted);

        size_t row_term_count = 0;
        for (const auto &elem : elements) {
            row_term_count += ColumnInverter::InvertColumn(begin_doc_id + i, elem);
        }
        column_lengths[i] = row_term_count;
        term_count_sum += row_term_count;
    }
    column_lengths_.SetBatch(begin_doc_id, column_lengths);
    return term_count_sum;
}

} // namespace infinity