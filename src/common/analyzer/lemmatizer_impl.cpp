// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include <cassert>

module infinity_core:lemmatizer.impl;

import :stl;
import :lemmatizer;

import std;

namespace fs = std::filesystem;

namespace infinity {

static const std::string ADJ = "a";
static const std::string ADJ_SAT = "s";
static const std::string ADV = "r";
static const std::string NOUN = "n";
static const std::string VERB = "v";

Lemmatizer::Lemmatizer(const std::string &path) : path_(path) {}

Lemmatizer::~Lemmatizer() {}

Status Lemmatizer::Load() {
    file_map_ = {{ADJ, "adj"}, {ADV, "adv"}, {NOUN, "noun"}, {VERB, "verb"}};
    pos_numbers_ = {{NOUN, 1}, {VERB, 2}, {ADJ, 3}, {ADV, 4}, {ADJ_SAT, 5}};
    MORPHOLOGICAL_SUBSTITUTIONS = {
        {NOUN, {{"s", ""}, {"ses", "s"}, {"ves", "f"}, {"xes", "x"}, {"zes", "z"}, {"ches", "ch"}, {"shes", "sh"}, {"men", "man"}, {"ies", "y"}}},
        {VERB, {{"s", ""}, {"ies", "y"}, {"es", "e"}, {"es", ""}, {"ed", "e"}, {"ed", ""}, {"ing", "e"}, {"ing", ""}}},
        {ADJ, {{"er", ""}, {"est", ""}, {"er", "e"}, {"est", "e"}}},
        {ADV, {}},
        {ADJ_SAT, {{"er", ""}, {"est", ""}, {"er", "e"}, {"est", "e"}}}};

    pos_names_ = [this] {
        std::unordered_map<int, std::string> names;
        for (const auto &pair : pos_numbers_) {
            names[pair.second] = pair.first;
        }
        return names;
    }();

    POS_LIST = {NOUN, VERB, ADJ, ADV};

    LoadLemmaPosOffsetMap();
    LoadExceptionMap();

    return Status::OK();
}

Status Lemmatizer::LoadLemmaPosOffsetMap() {
    fs::path root(path_);
    for (const auto &pair : file_map_) {
        const std::string &pos = pair.second;
        fs::path dict_path(root / ("index." + pos));

        std::ifstream file(dict_path.string());
        if (!file.is_open()) {
            return Status::InvalidAnalyzerFile(fmt::format("Failed to load wordnet lemmatizer, index.{}", pos));
        }

        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == ' ') {
                continue; // Skip comment lines or empty lines
            }

            std::istringstream stream(line);
            try {
                std::string lemma;
                std::string pos;
                std::getline(stream, lemma, ' ');
                std::getline(stream, pos, ' ');

                int n_synsets;
                stream >> n_synsets;
                assert(n_synsets > 0);

                int n_pointers;
                stream >> n_pointers;
                // Ignore pointer symbols
                for (int i = 0; i < n_pointers; ++i) {
                    std::string pointer_symbol;
                    stream >> pointer_symbol; // Dummy read
                }

                int n_senses;
                stream >> n_senses;
                assert(n_synsets == n_senses);

                // Ignore the number of senses ranked by frequency
                int n_ranked_senses;
                stream >> n_ranked_senses;

                // Get synset offsets
                std::vector<int> synset_offsets(n_synsets);
                for (int i = 0; i < n_synsets; ++i) {
                    stream >> synset_offsets[i];
                }

                // std::map lemmas and parts of speech to synsets
                lemma_pos_offset_map_[lemma][pos] = synset_offsets;

                if (pos == ADJ) {
                    // Duplicate all adjectives indiscriminately
                    lemma_pos_offset_map_[lemma][ADJ_SAT] = synset_offsets;
                }

            } catch (const std::exception &) {
                return Status::InvalidAnalyzerFile("Failed to load wordnet lemmatizer");
            }
        }
    }
    return Status::OK();
}

void Lemmatizer::LoadExceptionMap() {
    fs::path root(path_);
    for (const auto &pair : file_map_) {
        const auto &pos = pair.first;
        const auto &suffix = pair.second;
        fs::path dict_path(root / (suffix + ".exc"));

        std::ifstream file(dict_path.string());
        exception_map_[pos] = {};

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::string term;
            stream >> term;
            std::vector<std::string> exceptions{std::istream_iterator<std::string>{stream}, std::istream_iterator<std::string>{}};
            exception_map_[pos][term] = exceptions;
        }
    }
    exception_map_[ADJ_SAT] = exception_map_.at(ADJ);
}

std::vector<std::string> Lemmatizer::ApplyRules(const std::vector<std::string> &forms, const std::string &pos) {
    const auto &substitutions = MORPHOLOGICAL_SUBSTITUTIONS.at(pos);
    std::vector<std::string> results;
    for (const auto &form : forms) {
        for (const auto &[old, new_suffix] : substitutions) {
            if (form.size() >= old.size() && form.compare(form.size() - old.size(), old.size(), old) == 0) {
                results.push_back(form.substr(0, form.size() - old.size()) + new_suffix);
            }
        }
    }
    return results;
}

std::vector<std::string> Lemmatizer::FilterForms(const std::vector<std::string> &forms, const std::string &pos) {
    std::vector<std::string> result;
    Set<std::string> seen;
    for (const auto &form : forms) {
        // Check if form exists in lemma_pos_offset_map_
        if (lemma_pos_offset_map_.find(form) != lemma_pos_offset_map_.end()) {
            if (lemma_pos_offset_map_[form].find(pos) != lemma_pos_offset_map_[form].end()) {
                if (seen.find(form) == seen.end()) {
                    result.push_back(form);
                    seen.insert(form);
                }
            }
        }
    }
    return result;
}

std::vector<std::string> Lemmatizer::Morphy(const std::string &form, const std::string &pos, bool check_exceptions) {
    const auto &exceptions = exception_map_.at(pos);

    std::vector<std::string> forms;

    if (check_exceptions && exceptions.find(form) != exceptions.end()) {
        forms = exceptions.at(form);
    } else {
        forms = ApplyRules({form}, pos);
    }

    forms.push_back(form); // Include original form
    return FilterForms(forms, pos);
}

std::string Lemmatizer::Lemmatize(const std::string &form, const std::string &pos) {
    // If pos is specified, create a vector with that pos, otherwise use all POS_LIST
    std::vector<std::string> parts_of_speech;
    if (!pos.empty()) {
        parts_of_speech.push_back(pos);
    } else {
        parts_of_speech = POS_LIST;
    }

    for (const auto &part : parts_of_speech) {
        auto analyses = Morphy(form, part);
        if (!analyses.empty()) {
            // Return the first successful analysis
            return analyses[0];
        }
    }

    return form;
}

} // namespace infinity