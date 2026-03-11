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

export module infinity_core:log_file;

import :infinity_type;
import :utility;

import std;

// responsible for parsing and generating filenames for catalog files and wal files

namespace infinity {

export struct TempWalFileInfo {
    std::string path_;
};

export struct WalFileInfo {
    std::string path_;
    TxnTimeStamp max_commit_ts_;

    auto operator<=>(const WalFileInfo &other) const {
        size_t path_num = std::stoull(Partition(path_, '.').back());
        size_t other_path_num = std::stoull(Partition(other.path_, '.').back());

        return path_num <=> other_path_num;
    }
};

export class WalFile {
public:
    static std::pair<std::optional<TempWalFileInfo>, std::vector<WalFileInfo>> ParseWalFilenames(const std::string &wal_dir);

    static std::string WalFilename(TxnTimeStamp max_commit_ts);

    static std::string TempWalFilename();

    // max_commit_ts is the largest commit ts before the latest checkpoint
    // static void RecycleWalFile(TxnTimeStamp max_commit_ts, const std::string &wal_dir);
};

} // namespace infinity
