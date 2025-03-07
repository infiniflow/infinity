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

export module table_index_meeta;

import stl;
import status;
import column_def;

namespace infinity {

class KVInstance;
class TableMeeta;
class IndexBase;

export class TableIndexMeeta {
public:
    TableIndexMeeta(String index_id_str, TableMeeta &table_meta, KVInstance &kv_instance);

    KVInstance &kv_instance() const { return kv_instance_; }

    TableMeeta &table_meta() const { return table_meta_; }

    const String &index_id_str() const { return index_id_str_; }

    Tuple<SharedPtr<IndexBase>, Status> GetIndexBase();

    SharedPtr<String> GetTableIndexDir();

    Tuple<SharedPtr<ColumnDef>, Status> GetColumnDef();

    Status GetSegmentIDs(Vector<SegmentID> *&segment_ids);

    Status SetSegmentIDs(const Vector<SegmentID> &segment_ids);

    Status AddSegmentID(SegmentID segment_id);

    Status InitSet(const SharedPtr<IndexBase> &index_base);

    Status UninitSet();

private:
    Status LoadIndexDef();

    Status LoadSegmentIDs();

    String GetTableIndexTag(const String &tag) const;

public:
    String FtIndexCacheTag() const;

private:
    KVInstance &kv_instance_;
    TableMeeta &table_meta_;
    String index_id_str_;

    SharedPtr<IndexBase> index_def_;
    Optional<Vector<SegmentID>> segment_ids_;
};

} // namespace infinity
