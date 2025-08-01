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

export module infinity_core:allocator;

import :stl;
import global_resource_usage;

namespace infinity {

export class Allocator {
public:
    static ptr_t allocate(SizeT bytes) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrRawMemCount("Allocator");
#endif
        return new char[bytes];
    }

    static void deallocate(ptr_t ptr) {
        delete[] ptr;
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrRawMemCount("Allocator");
#endif
    }
};

} // namespace infinity