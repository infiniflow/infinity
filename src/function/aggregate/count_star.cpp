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

import stl;
import catalog;

import infinity_exception;
import aggregate_function;
import aggregate_function_set;
import parser;
import third_party;

module count_star;

namespace infinity {

template <typename ResultType>
struct CountStarState {
    i64 value_{};

    void Initialize() { this->value_ = 0; }

    void Update(i64 *__restrict input, SizeT idx) { value_ = input[idx]; }

    inline void ConstantUpdate(i64 *__restrict input, SizeT idx, SizeT) { value_ = input[idx]; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

void RegisterCountStarFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "COUNT_STAR";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction count_function =
            CountStarAggregate<CountStarState<BigIntT>, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
