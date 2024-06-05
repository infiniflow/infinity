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

export module special_function;

import stl;
import function;
import data_type;
import internal_types;

namespace infinity {

export enum class SpecialType {
    kRowID,
    kDistance,
    kSimilarity,
    kScore,
    kCreateTs,
    kDeleteTs,
};

export class SpecialFunction final : public Function {
public:
    SpecialFunction(const String func_name, DataType data_type, SizeT extra_idx, SpecialType special_type)
        : Function(std::move(func_name), FunctionType::kSpecial), data_type_(std::move(data_type)), special_type_(special_type), extra_idx_(extra_idx) {}

    const String &name() const { return name_; }

    const DataType &data_type() const { return data_type_; }

    SizeT extra_idx() { return extra_idx_; }

    FunctionType type() { return type_; }

    SpecialType special_type() { return special_type_; }

    String ToString() const final { return name(); }

private:
    DataType data_type_;
    SpecialType special_type_;
    SizeT extra_idx_{0};
};

} // namespace infinity
