// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module table_meeta;

import stl;
import status;
import column_def;

namespace infinity {

class KVInstance;

export class TableMeeta {
public:
    TableMeeta(String table_id_str, KVInstance &kv_instance);

    KVInstance &kv_instance() const { return kv_instance_; }

    const String &table_id_str() const { return table_id_str_; }

    Status GetColumnDefs(Vector<SharedPtr<ColumnDef>> *&column_defs) {
        if (!column_defs_) {
            auto status = LoadColumnDefs();
            if (!status.ok()) {
                return status;
            }
        }
        column_defs = &column_defs_.value();
        return Status::OK();
    }

    Status GetSegmentIDs(Vector<SegmentID> *&segment_ids) {
        if (!segment_ids_) {
            auto status = LoadSegmentIDs();
            if (!status.ok()) {
                return status;
            }
        }
        segment_ids = &segment_ids_.value();
        return Status::OK();
    }

    Status GetNextSegmentID(SegmentID &next_segment_id) {
        if (!next_segment_id_) {
            auto status = LoadNextSegmentID();
            if (!status.ok()) {
                return status;
            }
        }
        next_segment_id = *next_segment_id_;
        return Status::OK();
    }

    Status GetTableDir(String *&table_dir) {
        if (!table_dir_) {
            auto status = LoadTableDir();
            if (!status.ok()) {
                return status;
            }
        }
        table_dir = &table_dir_.value();
        return Status::OK();
    }

    Status SetSegmentIDs(const Vector<SegmentID> &segment_ids);

    Status AddSegmentID(SegmentID segment_id);

    Status SetNextBlockID(SegmentID next_segment_id);

    Status InitSet();

private:
    Status LoadColumnDefs();

    Status LoadSegmentIDs();

    Status LoadNextSegmentID();

    Status LoadTableDir();

    String GetTableTag(const String &tag) const;

private:
    KVInstance &kv_instance_;
    String table_id_str_;

    Optional<Vector<SharedPtr<ColumnDef>>> column_defs_;

    Optional<Vector<SegmentID>> segment_ids_;
    Optional<SegmentID> next_segment_id_;
    Optional<String> table_dir_;

    // String db_id_str_;
    // String table_id_str_;
    // String table_dir_;
    // Vector<SharedPtr<ColumnDef>> column_defs_;

public:
    String db_id_str_;
};

} // namespace infinity
