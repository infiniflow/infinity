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
import third_party;
import catalog;
import buffer_manager;
import file_system;
import parser;
import file_writer;
import file_reader;

export module pgm_numeric;

namespace infinity {

struct ApproxPos {
    SizeT pos_;         ///< The approximate position of the key.
    SizeT lower_bound_; ///< The lower bound of the range.
    SizeT upper_bound_; ///< The upper bound of the range.
};

class PGMBase {
public:
    virtual ~PGMBase() = default;

    virtual void Load(SharedPtr<FileReader> &file_reader) = 0;
    virtual void Save(SharedPtr<FileWriter> &file_writer) const = 0;
    virtual void AppendBlock(const void *data, SizeT size) = 0;
    virtual void Build() = 0;
    virtual ApproxPos Search(u64 val) const = 0;
};

export class NumericIndex {
public:
    NumericIndex();

    void Insert(SegmentEntry *segment_entry, SharedPtr<ColumnDef> column_def, BufferManager *buffer_mgr);

private:
    void CreatePGM(LogicalType logical_type);

private:
    SharedPtr<PGMBase> column_index_;
};

} // namespace infinity