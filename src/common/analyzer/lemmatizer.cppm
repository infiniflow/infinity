// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//
//     https://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module lemmatizer;

import stl;
import status;

namespace infinity {

export class Lemmatizer {
public:
    Lemmatizer(const String &path);

    ~Lemmatizer();

    Status Load();

    String Lemmatize(const String &form, const String &pos = "");

private:
    Status LoadLemmaPosOffsetMap();

    void LoadExceptionMap();

    Vector<String> Morphy(const String &form, const String &pos, bool check_exceptions = true);

    Vector<String> ApplyRules(const Vector<String> &forms, const String &pos);

    Vector<String> FilterForms(const Vector<String> &forms, const String &pos);

    String path_;

    HashMap<String, HashMap<String, Vector<int>>> lemma_pos_offset_map_;
    HashMap<String, HashMap<String, Vector<String>>> exception_map_;
    HashMap<String, int> pos_numbers_;
    HashMap<int, String> pos_names_;
    HashMap<String, String> file_map_;
    HashMap<String, Vector<Pair<String, String>>> MORPHOLOGICAL_SUBSTITUTIONS;
    Vector<String> POS_LIST;
};

} // namespace infinity