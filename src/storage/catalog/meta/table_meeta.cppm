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
class TableDef;

export class TableMeeta {
public:
    TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance &kv_instance);

    KVInstance &kv_instance() const { return kv_instance_; }

    const String &table_id_str() const { return table_id_str_; }
    const String &db_id_str() const { return db_id_str_; }

    Status GetIndexIDs(Vector<String> *&index_id_strs, Vector<String> **index_names = nullptr);

    Status InitSet(SharedPtr<TableDef> table_def);

    Status UninitSet();

    Status GetNextSegmentID(SegmentID &next_segment_id) {
        if (!next_segment_id_) {
            Status status = LoadNextSegmentID();
            if (!status.ok()) {
                return status;
            }
        }
        next_segment_id = *next_segment_id_;
        return Status::OK();
    }

    Status SetNextSegmentID(SegmentID next_segment_id);

    Status GetUnsealedSegmentID(SegmentID &unsealed_segment_id) {
        if (!unsealed_segment_id_) {
            Status status = LoadUnsealedSegmentID();
            if (!status.ok()) {
                return status;
            }
        }
        unsealed_segment_id = *unsealed_segment_id_;
        return Status::OK();
    }

    Status SetUnsealedSegmentID(SegmentID unsealed_segment_id);

    Status SetSegmentIDs(const Vector<SegmentID> &segment_ids);

    Status AddSegmentID(SegmentID segment_id);

    Tuple<ColumnID, Status> GetColumnIDByColumnName(const String &column_name);
    SharedPtr<String> GetTableDir();
    Tuple<SharedPtr<Vector<SegmentID>>, Status> GetSegmentIDs();
    Tuple<SharedPtr<Vector<SharedPtr<ColumnDef>>>, Status> GetColumnDefs();
    Tuple<SharedPtr<ColumnDef>, Status> GetColumnDefByColumnName(const String &column_name);

private:
    Status LoadColumnDefs();

    Status LoadSegmentIDs();

    Status LoadIndexIDs();

    Status LoadNextSegmentID();

    Status LoadUnsealedSegmentID();

    String GetTableTag(const String &tag) const;

private:
    KVInstance &kv_instance_;
    String db_id_str_;
    String table_id_str_;

    Optional<Vector<SharedPtr<ColumnDef>>> column_defs_;
    Optional<Vector<SegmentID>> segment_ids_;
    Optional<Vector<String>> index_id_strs_;
    Optional<Vector<String>> index_names_;
    Optional<SegmentID> next_segment_id_;
    Optional<SegmentID> unsealed_segment_id_;
};

} // namespace infinity
