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

export module infinity_core:explain_fragment;

import :stl;
import :plan_fragment;

namespace infinity {

export class ExplainFragment {
public:
    static void Explain(PlanFragment *op, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);
};

} // namespace infinity
