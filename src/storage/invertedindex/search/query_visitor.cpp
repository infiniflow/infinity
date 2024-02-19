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

#include "query.h"
#include <vector>

module query_visitor;

import stl;
import index_defines;
import index_config;

namespace infinity {

template <>
void QueryVisitor::Visit<And>(And &) {}

template <>
void QueryVisitor::Visit<AndNot>(AndNot &) {}

template <>
void QueryVisitor::Visit<Or>(Or &) {}

template <>
void QueryVisitor::Visit<Wand>(Wand &) {}

} // namespace infinity
