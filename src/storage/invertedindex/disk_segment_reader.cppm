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

export module infinity_core:disk_index_segment_reader;

import :stl;
import :segment_posting;
import :index_defines;
import :index_segment_reader;
import :dict_reader;
import :file_reader;
import :posting_list_format;
import :term_meta;

import internal_types;

namespace infinity {
export class DiskIndexSegmentReader : public IndexSegmentReader {
public:
    DiskIndexSegmentReader(SegmentID segment_id,
                           ChunkID chunk_id,
                           const std::string &index_dir,
                           const std::string &base_name,
                           RowID base_row_id,
                           optionflag_t flag);

    virtual ~DiskIndexSegmentReader();

    bool GetSegmentPosting(const std::string &term, SegmentPosting &seg_posting, bool fetch_position = true) const override;

private:
    RowID base_row_id_{INVALID_ROWID};
    std::shared_ptr<DictionaryReader> dict_reader_;
    std::string posting_file_{};
    std::string posting_file_obj_{};
    std::string dict_file_{};
    u8 *data_ptr_{};
    size_t data_len_{};
};

} // namespace infinity