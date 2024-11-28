module;

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

module ik_dict;

import ik_dict_segment;
import hit;
import stl;
import status;

namespace fs = std::filesystem;

namespace infinity {

const String PATH_DIC_MAIN = "ik/main.dic";
const String PATH_DIC_SURNAME = "ik/surname.dic";
const String PATH_DIC_QUANTIFIER = "ik/quantifier.dic";
const String PATH_DIC_SUFFIX = "ik/suffix.dic";
const String PATH_DIC_PREP = "ik/preposition.dic";
const String PATH_DIC_STOP = "ik/stopword.dic";
const String FILE_NAME = "ik/IKAnalyzer.cfg.xml";
const String EXT_DICT = "ik/ext_dict";
const String EXT_STOP = "ik/ext_stopwords";

Dictionary::Dictionary(const String &dir) : conf_dir_(dir) {}

Status Dictionary::Load() {
    Status load_status;
    fs::path root(conf_dir_);
    fs::path config_file = root / FILE_NAME;

    std::ifstream input(config_file);
    if (!input.is_open()) {
        return Status::InvalidAnalyzerFile(config_file);
    }

    if (input.is_open()) {
        std::stringstream buffer;
        buffer << input.rdbuf();
        String content = buffer.str();
        ParseProperties(content);
    }

    load_status = LoadMainDict();
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadSurnameDict();
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadQuantifierDict();
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadSuffixDict();
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadPrepDict();
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadStopWordDict();
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
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

Status Dictionary::LoadDictFile(DictSegment *dict, const String &file_path, bool critical, const String &name) {
    fs::path file(file_path);
    std::wifstream is(file);
    if (!is.is_open()) {
        return Status::InvalidAnalyzerFile(file_path);
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
    return Status::OK();
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

Status Dictionary::LoadMainDict() {
    main_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_MAIN).string();
    Status load_status = LoadDictFile(main_dict_.get(), file, false, "Main Dict");
    if (!load_status.ok()) {
        return load_status;
    }
    load_status = LoadExtDict();
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
}

Status Dictionary::LoadExtDict() {
    Vector<String> ext_dict_files = GetExtDictionarys();
    Status load_status;
    if (!ext_dict_files.empty()) {
        for (const String &ext_dict_name : ext_dict_files) {
            std::cout << "[Dict Loading] " << ext_dict_name << std::endl;
            String file = fs::path(conf_dir_) / fs::path(ext_dict_name).string();
            load_status = LoadDictFile(main_dict_.get(), file, false, "Extra Dict");
            if (!load_status.ok()) {
                return load_status;
            }
        }
    }
    return Status::OK();
}

Status Dictionary::LoadStopWordDict() {
    stop_words_ = MakeUnique<DictSegment>(L'\0');
    Status load_status;
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_STOP).string();
    load_status = LoadDictFile(stop_words_.get(), file, false, "Main Stopwords");
    if (!load_status.ok()) {
        return load_status;
    }

    Vector<String> ext_stopword_dict_files = GetExtStopWordDictionarys();
    if (!ext_stopword_dict_files.empty()) {
        for (const String &ext_stopword_dict_file : ext_stopword_dict_files) {
            std::cout << "[Dict Loading] " << ext_stopword_dict_file << std::endl;
            String file = fs::path(conf_dir_) / fs::path(ext_stopword_dict_file).string();
            load_status = LoadDictFile(stop_words_.get(), file, false, "Extra Stopwords");
            if (!load_status.ok()) {
                return load_status;
            }
        }
    }
    return Status::OK();
}

Status Dictionary::LoadQuantifierDict() {
    quantifier_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_QUANTIFIER).string();
    Status load_status = LoadDictFile(quantifier_dict_.get(), file, false, "Quantifier");
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
}

Status Dictionary::LoadSurnameDict() {
    surname_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_SURNAME).string();
    Status load_status = LoadDictFile(surname_dict_.get(), file, true, "Surname");
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
}

Status Dictionary::LoadSuffixDict() {
    suffix_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_SUFFIX).string();
    Status load_status = LoadDictFile(suffix_dict_.get(), file, true, "Suffix");
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
}

Status Dictionary::LoadPrepDict() {
    prep_dict_ = MakeUnique<DictSegment>(L'\0');
    String file = fs::path(conf_dir_) / fs::path(PATH_DIC_PREP).string();
    Status load_status = LoadDictFile(prep_dict_.get(), file, true, "Preposition");
    if (!load_status.ok()) {
        return load_status;
    }
    return Status::OK();
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