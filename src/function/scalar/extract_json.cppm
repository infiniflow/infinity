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

module;

import third_party;
import std.compat;
import function_expr;

export module infinity_core:extract_json;

namespace infinity {
class NewCatalog;
class DataBlock;
class ColumnVector;
export void JsonExtract(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &, std::shared_ptr<ColumnVector> &);
export void JsonExtractInt(std::shared_ptr<BaseExtraInfo> extract_ptr, const DataBlock &, std::shared_ptr<ColumnVector> &);
export void RegisterJsonFunction(NewCatalog *catalog_ptr);

} // namespace infinity
