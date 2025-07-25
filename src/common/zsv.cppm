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

extern "C" {
// Do not use "zsv/api.h" or "zsv/common.h" directly
#include "zsv.h"
}

export module infinity_core:zsv;

namespace infinity {

export using ZsvOpts = zsv_opts;

export using ZsvStatus = zsv_status;

export using ZsvCell = zsv_cell;

export class ZsvParser {
    zsv_parser parser_{};

public:
    ZsvParser() : parser_(nullptr) {}

    ZsvParser(ZsvOpts *opts) : parser_(zsv_new(opts)) {}

    ZsvParser(const ZsvParser &) = delete;

    ZsvParser(const ZsvParser &&) = delete;

    ZsvParser &operator=(const ZsvParser &) = delete;

    ZsvParser &operator=(ZsvParser &&other) {
        if (parser_) {
            zsv_delete(parser_);
        }
        parser_ = other.parser_;
        other.parser_ = nullptr;
        return *this;
    };

    ~ZsvParser() {
        if (parser_ != nullptr) {
            zsv_delete(parser_);
        }
    }

    ZsvStatus Finish() { return zsv_finish(parser_); }

    ZsvStatus ParseMore() { return zsv_parse_more(parser_); }

    static const char *ParseStatusDesc(ZsvStatus status) { return reinterpret_cast<const char *>(zsv_parse_status_desc(status)); }

    size_t CellCount() { return zsv_cell_count(parser_); }

    void Abort() { zsv_abort(parser_); }

    unsigned char *GetCellStr(size_t idx) { return zsv_get_cell_str(parser_, idx); }

    ZsvCell GetCell(size_t idx) { return zsv_get_cell(parser_, idx); }

    void SetRowHandler(void (*row_handler)(void *ctx)) { zsv_set_row_handler(parser_, row_handler); }
};
} // namespace infinity
