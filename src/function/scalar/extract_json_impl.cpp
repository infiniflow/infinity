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
    static void Execute(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
        auto extra_ptr = reinterpret_cast<const JsonExtraInfo *>(extract_ptr.get());
        auto column = input.column_vectors[0];
        auto input_data = reinterpret_cast<const JsonT *>(column->data());
        auto row_count = input.row_count();
        const std::shared_ptr<Bitmask> &output_null = output->nulls_ptr_;

        if (column->vector_type() == ColumnVectorType::kFlat) {
            for (size_t row_index = 0; row_index < row_count; row_index++) {
                const auto json_info = input_data[row_index];
                auto json_data = column->buffer_->GetVarchar(json_info.file_offset_, json_info.length_);
                std::vector<uint8_t> bson(reinterpret_cast<const uint8_t *>(json_data),
                                          reinterpret_cast<const uint8_t *>(json_data) + json_info.length_);
                auto json = JsonManager::from_bson(bson);
                auto [is_null, extracted_value] = ExtractFunc(json, extra_ptr->json_tokens_);
                output_null->Set(row_index, !is_null);
                Value v = MakeValueFunc(extracted_value);
                output->AppendValue(v);
            }
            output->Finalize(row_count);
        } else {
            RecoverableError(Status::SyntaxError("JsonExtract: Invalid column type."));
        }
    }
};

void JsonExtractString(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<std::string, JsonManager::json_extract, Value::MakeVarchar>::Execute(extract_ptr, input, output);
}

void JsonExtractInt(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<int, JsonManager::json_extract_int, Value::MakeInt>::Execute(extract_ptr, input, output);
}

void JsonExtractDouble(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<double, JsonManager::json_extract_double, Value::MakeDouble>::Execute(extract_ptr, input, output);
}

void JsonExtractBool(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<bool, JsonManager::json_extract_bool, Value::MakeBool>::Execute(extract_ptr, input, output);
}

void JsonExtractIsNull(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &input, std::shared_ptr<ColumnVector> &output) {
    JsonExtractor<bool, JsonManager::json_extract_is_null, Value::MakeBool>::Execute(extract_ptr, input, output);
}

void RegisterJsonFunction(NewCatalog *catalog_ptr) {
    {
        std::string func_name = "json_extract";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kVarchar), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_string";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kVarchar), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_int";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kInteger), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_double";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kDouble), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_bool";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kBoolean), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
    {
        std::string func_name = "json_extract_isnull";
        std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);
        ScalarFunction json_extract(func_name, {DataType(LogicalType::kJson)}, DataType(LogicalType::kBoolean), JsonExtractBase);
        function_set_ptr->AddFunction(json_extract);
        NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
    }
}

} // namespace infinity