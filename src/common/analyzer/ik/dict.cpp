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
    conf_dir = dir;
    fs::path root(conf_dir);
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
    Vector<String> extDictFiles;
    String extDictCfg = GetProperty(EXT_DICT);
    if (!extDictCfg.empty()) {
        std::stringstream ss(extDictCfg);
        String filePath;
        while (std::getline(ss, filePath, ';')) {
            if (!filePath.empty()) {
                String path = fs::path(conf_dir) / fs::path(filePath).string();
                WalkFileTree(extDictFiles, path);
            }
        }
    }
    return extDictFiles;
}

Vector<String> Dictionary::GetExtStopWordDictionarys() {
    Vector<String> extStopWordDictFiles;
    String extStopWordDictCfg = GetProperty(EXT_STOP);
    if (!extStopWordDictCfg.empty()) {
        std::stringstream ss(extStopWordDictCfg);
        String filePath;
        while (std::getline(ss, filePath, ';')) {
            if (!filePath.empty()) {
                String path = fs::path(conf_dir) / fs::path(filePath).string();
                WalkFileTree(extStopWordDictFiles, path);
            }
        }
    }
    return extStopWordDictFiles;
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

Hit *Dictionary::MatchInMainDict(const Vector<wchar_t> &charArray) { return main_dict_->Match(charArray); }

Hit *Dictionary::MatchInMainDict(const Vector<wchar_t> &charArray, int begin, int length) { return main_dict_->Match(charArray, begin, length); }

Hit *Dictionary::MatchInQuantifierDict(const Vector<wchar_t> &charArray, int begin, int length) {
    return quantifier_dict_->Match(charArray, begin, length);
}

Hit *Dictionary::MatchWithHit(const Vector<wchar_t> &charArray, int currentIndex, Hit *matchedHit) {
    DictSegment *ds = matchedHit->getMatchedDictSegment();
    return ds->Match(charArray, currentIndex, 1, matchedHit);
}

bool Dictionary::IsStopWord(const Vector<wchar_t> &charArray, int begin, int length) {
    return stop_words_->Match(charArray, begin, length)->IsMatch();
}

void Dictionary::LoadMainDict() {
    main_dict_ = new DictSegment(L'\0');
    String file = fs::path(conf_dir) / fs::path(PATH_DIC_MAIN).string();
    LoadDictFile(main_dict_, file, false, "Main Dict");
    LoadExtDict();
}

void Dictionary::LoadExtDict() {
    Vector<String> extDictFiles = GetExtDictionarys();
    if (!extDictFiles.empty()) {
        for (const String &extDictName : extDictFiles) {
            std::cout << "[Dict Loading] " << extDictName << std::endl;
            String file = fs::path(conf_dir) / fs::path(extDictName).string();
            LoadDictFile(main_dict_, file, false, "Extra Dict");
        }
    }
}

void Dictionary::LoadStopWordDict() {
    stop_words_ = new DictSegment(L'\0');

    String file = fs::path(conf_dir) / fs::path(PATH_DIC_STOP).string();
    LoadDictFile(stop_words_, file, false, "Main Stopwords");

    Vector<String> extStopWordDictFiles = GetExtStopWordDictionarys();
    if (!extStopWordDictFiles.empty()) {
        for (const String &extStopWordDictName : extStopWordDictFiles) {
            std::cout << "[Dict Loading] " << extStopWordDictName << std::endl;

            String file = fs::path(conf_dir) / fs::path(extStopWordDictName).string();
            LoadDictFile(stop_words_, file, false, "Extra Stopwords");
        }
    }
}

void Dictionary::LoadQuantifierDict() {
    quantifier_dict_ = new DictSegment(L'\0');
    String file = fs::path(conf_dir) / fs::path(PATH_DIC_QUANTIFIER).string();
    LoadDictFile(quantifier_dict_, file, false, "Quantifier");
}

void Dictionary::LoadSurnameDict() {
    DictSegment *_SurnameDict = new DictSegment(L'\0');
    String file = fs::path(conf_dir) / fs::path(PATH_DIC_SURNAME).string();
    LoadDictFile(_SurnameDict, file, true, "Surname");
}

void Dictionary::LoadSuffixDict() {
    DictSegment *_SuffixDict = new DictSegment(L'\0');
    String file = fs::path(conf_dir) / fs::path(PATH_DIC_SUFFIX).string();
    LoadDictFile(_SuffixDict, file, true, "Suffix");
}

void Dictionary::LoadPrepDict() {
    DictSegment *_PrepDict = new DictSegment(L'\0');
    String file = fs::path(conf_dir) / fs::path(PATH_DIC_PREP).string();
    LoadDictFile(_PrepDict, file, true, "Preposition");
}

void Dictionary::ParseProperties(const String &content) {
    std::stringstream ss(content);
    String line;
    while (std::getline(ss, line)) {
        size_t pos = line.find('=');
        if (pos != String::npos) {
            String key = line.substr(0, pos);
            String value = line.substr(pos + 1);
            props[key] = value;
        }
    }
}

} // namespace infinity