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

export module base_memindex;

import stl;
import memindex_tracer;

namespace infinity {

export struct BaseMemIndexInfo {
    SharedPtr<String> table_name_;
    SharedPtr<String> index_name_;
    SharedPtr<String> db_name_;
    SizeT row_count_;
};

export class BaseMemIndex {
public:
    MemIndexTracerInfo GetInfo() const {
        BaseMemIndexInfo info = GetInfoInner();
        return {info.table_name_, info.index_name_, info.db_name_, info.row_count_, mem_used_};
    }

protected:
    void AddMemUsed(SizeT mem) { mem_used_ += mem; }

    virtual BaseMemIndexInfo GetInfoInner() const = 0;

private:
    SizeT mem_used_ = 0;
};

} // namespace infinity
