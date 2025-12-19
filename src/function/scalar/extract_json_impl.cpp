// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:extract_json.impl;

import :extract_json;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :json_manager;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

void JsonExtractBase(const DataBlock &, std::shared_ptr<ColumnVector> &) {}

template <typename T, std::tuple<bool, T> (*ExtractFunc)(const JsonTypeDef &, const std::vector<std::string> &), Value (*MakeValueFunc)(T)>
class JsonExtractor {
public:
    static void Execute(const DataBlock &input_block, std::shared_ptr<ColumnVector> &output_column) {
        if (input_block.column_vectors.size() != 2) {
            RecoverableError(Status::SyntaxError("JsonExtract: Invalid column size."));
        }

        const auto &json_column = input_block.column_vectors[0];
        const auto &path_column = input_block.column_vectors[1];

        auto rvarchar = path_column->GetVarchar(0);
        auto [is_valid, tokens] = JsonManager::get_json_tokens(std::string(rvarchar.data(), rvarchar.size()));
        if (!is_valid) {
            RecoverableError(Status::SyntaxError("JsonExtract: Invalid json path."));
        }

        auto json_column_data = reinterpret_cast<const JsonT *>(json_column->data());
        auto row_count = input_block.row_count();
        const std::shared_ptr<Bitmask> &output_null = output_column->nulls_ptr_;

        if (json_column->vector_type() == ColumnVectorType::kFlat) {
            for (size_t row_index = 0; row_index < row_count; row_index++) {
                const auto json_info = json_column_data[row_index];
                auto json_data = json_column->buffer_->GetVarchar(json_info.file_offset_, json_info.length_);
                std::vector<uint8_t> bson(reinterpret_cast<const uint8_t *>(json_data),
                                          reinterpret_cast<const uint8_t *>(json_data) + json_info.length_);
                auto json = JsonManager::from_bson(bson);
                auto [is_null, extracted_value] = ExtractFunc(json, tokens);
                output_null->Set(row_index, !is_null);
                Value v = MakeValueFunc(extracted_value);
                output_column->AppendValue(v);
            }
            output_column->Finalize(row_count);
        } else {
            RecoverableError(Status::SyntaxError("JsonExtract: Invalid column type."));
        }
    }
};

void JsonExtractString(const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<std::string, JsonManager::json_extract, Value::MakeVarchar>::Execute(input, output);
}

void JsonExtractInt(const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<int, JsonManager::json_extract_int, Value::MakeInt>::Execute(input, output);
}

void JsonExtractDouble(const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<double, JsonManager::json_extract_double, Value::MakeDouble>::Execute(input, output);
}

void JsonExtractBool(const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<bool, JsonManager::json_extract_bool, Value::MakeBool>::Execute(input, output);
}

void JsonExtractIsNull(const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<bool, JsonManager::json_extract_is_null, Value::MakeBool>::Execute(input, output);
}

void RegisterJsonFunction(NewCatalog *catalog_ptr) {
    {
        std::string func_name = "json_extract";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kVarchar),
                                    JsonExtractString);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_string";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kVarchar),
                                    JsonExtractString);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_int";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kInteger),
                                    JsonExtractInt);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_double";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kDouble),
                                    JsonExtractDouble);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_bool";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kBoolean),
                                    JsonExtractBool);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_isnull";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name,
                                    {DataType(LogicalType::kJson), DataType(LogicalType::kVarchar)},
                                    DataType(LogicalType::kBoolean),
                                    JsonExtractIsNull);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
}

} // namespace infinity