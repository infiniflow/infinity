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

export module query_options;

import stl;
import extra_ddl_info;
import statement_common;

namespace infinity {

export class CreateDatabaseOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
};

export class DropDatabaseOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
};

export class CreateTableOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
    Vector<InitParameter *> properties_;
};

export class DropTableOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
};

export class CreateIndexOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
};

export class DropIndexOptions {
public:
    ConflictType conflict_type_{ConflictType::kError};
};

export class ImportOptions {
public:
    char delimiter_{','};
    bool header_{false};
    CopyFileType copy_file_type_{CopyFileType::kCSV};
};

export class ExportOptions {
public:
    char delimiter_{','};
    bool header_{false};
    CopyFileType copy_file_type_{CopyFileType::kCSV};
};

export class OptimizeOptions {
public:
    String index_name_{};
    Vector<InitParameter *> opt_params_{};
};

} // namespace infinity
