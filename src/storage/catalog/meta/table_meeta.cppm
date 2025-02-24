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
    TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance &kv_instance);

    KVInstance &kv_instance() const { return kv_instance_; }

    const String &table_id_str() const { return table_id_str_; }
    const String &db_id_str() const { return db_id_str_; }

    Status GetColumnDefs(Vector<SharedPtr<ColumnDef>> *&column_defs);

    Status GetSegmentIDs(Vector<SegmentID> *&segment_ids);

    Status GetNextSegmentID(SegmentID &next_segment_id);

    Status GetTableDir(String *&table_dir);

    Status GetColumnIDByColumnName(const String &column_name, ColumnID &column_id);

    Status SetNextSegmentID(SegmentID next_segment_id);

    Status InitSet();

    // New
    Status Init();

    Tuple<SegmentID, Status> GetLatestSegmentID();

    Status SetLatestSegmentID(SegmentID next_segment_id);

    Status AddSegmentID(SegmentID segment_id);

    Tuple<ColumnID, Status> GetColumnIDByColumnName(const String &column_name);
    Tuple<SharedPtr<String>, Status> GetTableDir();
    Tuple<SharedPtr<Vector<SegmentID>>, Status> GetSegmentIDs();
    Tuple<SharedPtr<Vector<SharedPtr<ColumnDef>>>, Status> GetColumnDefs();

private:
    Status LoadColumnDefs();

    Status LoadSegmentIDs();

    Status LoadNextSegmentID();

    Status LoadLatestSegmentID();

    Status LoadTableDir();

    String GetTableTag(const String &tag) const;

    Status SetSegmentIDs(const Vector<SegmentID> &segment_ids);

    Status SetTableDir(const String &dir);

private:
    KVInstance &kv_instance_;
    String db_id_str_;
    String table_id_str_;

    Optional<Vector<SharedPtr<ColumnDef>>> column_defs_;
    Optional<Vector<SegmentID>> segment_ids_;
    Optional<SegmentID> next_segment_id_;
    Optional<SegmentID> latest_segment_id_;
    Optional<String> table_dir_;

    Set<SegmentID> segment_id_set_;
    Optional<SegmentID> current_segment_id_;
};

} // namespace infinity
