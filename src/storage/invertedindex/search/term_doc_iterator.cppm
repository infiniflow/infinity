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

export module term_doc_iterator;

import stl;
import memory_pool;
import posting_iterator;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import doc_iterator;
namespace infinity {
export class TermDocIterator : public DocIterator {
public:
    TermDocIterator(const String &term, PostingIterator *iter);

    virtual ~TermDocIterator();

    void AddIterator(DocIterator *iter) override {}

    void DoSeek(docid_t doc_id) override;

private:
    PostingIterator *iter_ = nullptr;
};
} // namespace infinity
