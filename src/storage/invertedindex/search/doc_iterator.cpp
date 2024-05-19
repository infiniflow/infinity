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

module doc_iterator;
import stl;
import memory_pool;
import index_defines;
import internal_types;
import term_doc_iterator;
import phrase_doc_iterator;
import third_party;
namespace infinity {

// for term iter
void DocIterator::PrepareFirstDoc() {
    if (GetType() == DocIteratorType::kTermIterator) {
        auto term_doc_iter = dynamic_cast<TermDocIterator *>(this);
        if (term_doc_iter) {
            term_doc_iter->DoSeek(0);
        }
    } else if (GetType() == DocIteratorType::kPhraseIterator) {
        auto phrase_doc_iter = dynamic_cast<PhraseDocIterator *>(this);
        if (phrase_doc_iter) {
            phrase_doc_iter->DoSeek(0);
        }
    }
    // do nothing for normal "or", "and", "and not" iter
}

} // namespace infinity
