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

module infinity_core:index_segment_reader.impl;

import :index_segment_reader;

import :stl;

import :segment_posting;
import :index_defines;

namespace infinity {
IndexSegmentReader::IndexSegmentReader(SegmentID segment_id, ChunkID chunk_id) : segment_id_(segment_id), chunk_id_(chunk_id) {}
} // namespace infinity
