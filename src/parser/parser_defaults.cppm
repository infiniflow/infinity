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

#include "default_value.h"

export module parser_defaults;

namespace infinity {

export constexpr int64_t PARSER_EMBEDDING_LIMIT = PARSER_EMBEDDING_LIMIT_INTERNAL;
export constexpr int64_t PARSER_MAX_BITMAP_SIZE = PARSER_MAX_BITMAP_SIZE_INTERNAL;

} // namespace infinity
