module;

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

module ik_dict;

import ik_dict_segment;
import hit;
import stl;

namespace fs = std::filesystem;

namespace infinity {

const String PATH_DIC_MAIN = "main.dic";
const String PATH_DIC_SURNAME = "surname.dic";
const String PATH_DIC_QUANTIFIER = "quantifier.dic";
const String PATH_DIC_SUFFIX = "suffix.dic";
const String PATH_DIC_PREP = "preposition.dic";
const String PATH_DIC_STOP = "stopword.dic";
const String FILE_NAME = "IKAnalyzer.cfg.xml";
const String EXT_DICT = "ext_dict";
const String EXT_STOP = "ext_stopwords";

Dictionary::Dictionary(const String &dir) {
    conf_dir_ = dir;
    fs::path root(conf_dir_);
    fs::path configFile = root / FILE_NAME;

    std::ifstream input(configFile);
    if (!input.is_open()) {
        throw std::runtime_error("Config file not found: " + configFile.string());
    }

    if (input.is_open()) {
        std::stringstream buffer;
        buffer << input.rdbuf();
        String content = buffer.str();
        ParseProperties(content);
    }
}

void Dictionary::Initial() {
    LoadMainDict();
    LoadSurnameDict();
    LoadQuantifierDict();
    LoadSuffixDict();
    LoadPrepDict();
    LoadStopWordDict();
}

void Dictionary::WalkFileTree(Vector<String> &files, const String &path_str) {
    fs::path path(path_str);
    if (fs::is_regular_file(path)) {
        files.push_back(path.string());
    } else if (fs::is_directory(path)) {
        for (const auto &entry : fs::directory_iterator(path)) {
            WalkFileTree(files, entry.path().string());
        }
    } else {
        std::cerr << "[Ext Loading] file not found: " << path << std::endl;
    }
}

void Dictionary::LoadDictFile(DictSegment *dict, const String &file_path, bool critical, const String &name) {
    fs::path file(file_path);
    std::wifstream is(file);
    if (!is.is_open()) {
        std::cerr << "ik-analyzer: " << name << " not found" << std::endl;
        if (critical)
            throw std::runtime_error("ik-analyzer: " + name + " not found!!!");
        return;
    }

    std::wstring word;
    while (std::getline(is, word)) {
        if (!word.empty() && word[0] == L'\uFEFF') {
            word = word.substr(1);
        }
        if (!word.empty()) {
            dict->FillSegment(Vector<wchar_t>(word.begin(), word.end()));
        }
    }
}

Vector<String> Dictionary::GetExtDictionarys() {
    Vector<String> ext_dict_files;
    String ext_dict_cfg = GetProperty(EXT_DICT);
    if (!ext_dict_cfg.empty()) {
        std::stringstream ss(ext_dict_cfg);
        String file_path;
        while (std::getline(ss, file_path, ';')) {
            if (!file_path.empty()) {
                String path = fs::path(conf_dir_) / fs::path(file_path).string();
                WalkFileTree(ext_dict_files, path);
            }
        }
    }
    return ext_dict_files;
}

Vector<String> Dictionary::GetExtStopWordDictionarys() {
    Vector<String> ext_stopword_dict_files;
    String ext_stopword_dict_cfg = GetProperty(EXT_STOP);
    if (!ext_stopword_dict_cfg.empty()) {
        std::stringstream ss(ext_stopword_dict_cfg);
        String file_path;
        while (std::getline(ss, file_path, ';')) {
            if (!file_path.empty()) {
                String path = fs::path(conf_dir_) / fs::path(file_path).string();
                WalkFileTree(ext_stopword_dict_files, path);
            }
        }
    }
    return ext_stopword_dict_files;
}

void Dictionary::AddWords(const Vector<String> &words) {
    if (!words.empty()) {
        for (const String &word : words) {
            if (!word.empty()) {
                main_dict_->FillSegment(Vector<wchar_t>(word.begin(), word.end()));
            }
        }
    }
}

void Dictionary::DisableWords(const Vector<String> &words) {
    if (!words.empty()) {
        for (const String &word : words) {
            if (!word.empty()) {
                main_dict_->DisableSegment(Vector<wchar_t>(word.begin(), word.end()));
            }
        }
    }
}

Hit *Dictionary::MatchInMainDict(const Vector<wchar_t> &char_array) { return main_dict_->Match(char_array); }

Hit *Dictionary::MatchInMainDict(const Vector<wchar_t> &char_array, int begin, int length) { return main_dict_->Match(char_array, begin, length); }

Hit *Dictionary::MatchInQuantifierDict(const Vector<wchar_t> &char_array, int begin, int length) {
    return quantifier_dict_->Match(char_array, begin, length);
}

Hit *Dictionary::MatchWithHit(const Vector<wchar_t> &char_array, int current_index, Hit *matched_hit) {
    DictSegment *ds = matched_hit->getMatchedDictSegment();
    return ds->Match(char_array, current_index, 1, matched_hit);
}

bool Dictionary::IsStopWord(const Vector<wchar_t> &char_array, int begin, int length) {
    return stop_words_->Match(char_array, begin, length)->IsMatch();
}

void Dictionary::LoadMainDict() {
    main_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_MAIN).string();
    LoadDictFile(main_dict_.get(), file, false, "Main Dict");
    LoadExtDict();
}

void Dictionary::LoadExtDict() {
    Vector<String> ext_dict_files = GetExtDictionarys();
    if (!ext_dict_files.empty()) {
        for (const String &ext_dict_name : ext_dict_files) {
            std::cout << "[Dict Loading] " << ext_dict_name << std::endl;
            String file = fs::path(conf_dir_) / fs::path(ext_dict_name).string();
            LoadDictFile(main_dict_.get(), file, false, "Extra Dict");
        }
    }
}

void Dictionary::LoadStopWordDict() {
    stop_words_ = MakeUnique<DictSegment>(L'\0');

    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_STOP).string();
    LoadDictFile(stop_words_.get(), file, false, "Main Stopwords");

    Vector<String> ext_stopword_dict_files = GetExtStopWordDictionarys();
    if (!ext_stopword_dict_files.empty()) {
        for (const String &ext_stopword_dict_file : ext_stopword_dict_files) {
            std::cout << "[Dict Loading] " << ext_stopword_dict_file << std::endl;

            String file = fs::path(conf_dir_) / fs::path(ext_stopword_dict_file).string();
            LoadDictFile(stop_words_.get(), file, false, "Extra Stopwords");
        }
    }
}

void Dictionary::LoadQuantifierDict() {
    quantifier_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_QUANTIFIER).string();
    LoadDictFile(quantifier_dict_.get(), file, false, "Quantifier");
}

void Dictionary::LoadSurnameDict() {
    surname_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_SURNAME).string();
    LoadDictFile(surname_dict_.get(), file, true, "Surname");
}

void Dictionary::LoadSuffixDict() {
    suffix_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_SUFFIX).string();
    LoadDictFile(suffix_dict_.get(), file, true, "Suffix");
}

void Dictionary::LoadPrepDict() {
    prep_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_PREP).string();
    LoadDictFile(prep_dict_.get(), file, true, "Preposition");
}

void Dictionary::ParseProperties(const String &content) {
    std::stringstream ss(content);
    String line;
    while (std::getline(ss, line)) {
        size_t pos = line.find('=');
        if (pos != String::npos) {
            String key = line.substr(0, pos);
            String value = line.substr(pos + 1);
            props_[key] = value;
        }
    }
}

} // namespace infinity