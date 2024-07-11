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

export module simd_init;
import stl;

namespace infinity {

const Vector<char const *> &GetSupportedSimdTypesList();
bool IsSSE2Supported();
bool IsAVX2Supported();
bool IsAVX512Supported();

struct SIMD_FUNCTIONS {
    SIMD_FUNCTIONS();
};

const SIMD_FUNCTIONS &GetSIMD_FUNCTIONS();

} // namespace infinity
