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

export module infinity_core:dict_reader;

import :stl;
import :term_meta;
import :posting_list_format;
import :fst.fst;

namespace infinity {

export class DictionaryReader {
private:
    const std::string &dict_path_;
    TermMetaLoader meta_loader_;
    u8 *data_ptr_;
    size_t data_len_;
    std::unique_ptr<Fst> fst_;
    std::unique_ptr<FstStream> s_;

public:
    DictionaryReader(const std::string &dict_path, const PostingFormatOption &option);

    ~DictionaryReader();

    bool Lookup(const std::string &key, TermMeta &term_meta);

    void InitIterator(const std::string &min, const std::string &max);

    void InitIterator(const std::string &prefix);

    bool Next(std::string &term, TermMeta &term_meta);
};
} // namespace infinity
