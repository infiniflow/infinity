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

export module log_file;

import stl;

// responsible for parsing and generating filenames for catalog files and wal files

namespace infinity {

export struct FullCatalogFileInfo {
    String path_;
    TxnTimeStamp max_commit_ts_;
};

export struct DeltaCatalogFileInfo {
    String path_;
    TxnTimeStamp max_commit_ts_;
};

export struct TempWalFileInfo {
    String path_;
};

export struct WalFileInfo {
    String path_;
    TxnTimeStamp max_commit_ts_;
};

export class CatalogFile {
public:
    static Optional<Pair<FullCatalogFileInfo, Vector<DeltaCatalogFileInfo>>> ParseValidCheckpointFilenames(const String &catalog_dir,
                                                                                                           TxnTimeStamp max_checkpoint_ts);

    static String FullCheckpoingFilename(TxnTimeStamp max_commit_ts);

    static String TempFullCheckpointFilename(TxnTimeStamp max_commit_ts);

    static String DeltaCheckpointFilename(TxnTimeStamp max_commit_ts);

    // max_commit_ts is the largest commit ts before the latest full checkpoint
    static void RecycleCatalogFile(TxnTimeStamp max_commit_ts, const String &catalog_dir);

    static Pair<Vector<FullCatalogFileInfo>, Vector<DeltaCatalogFileInfo>> ParseCheckpointFilenames(const String &catalog_dir);
};

export class WalFile {
public:
    static Pair<Optional<TempWalFileInfo>, Vector<WalFileInfo>> ParseWalFilenames(const String &wal_dir);

    static String WalFilename(TxnTimeStamp max_commit_ts);

    static String TempWalFilename();

    // max_commit_ts is the largest commit ts before the latest checkpoint
    static void RecycleWalFile(TxnTimeStamp max_commit_ts, const String &wal_dir);
};

} // namespace infinity
