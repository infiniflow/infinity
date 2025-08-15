module;

export module infinity_core:ik_dict;

import :stl;
import :ik_dict_segment;
import :hit;
import :status;

namespace infinity {
export class Dictionary {
public:
    std::unique_ptr<DictSegment> main_dict_;
    std::unique_ptr<DictSegment> quantifier_dict_;
    std::unique_ptr<DictSegment> stop_words_;
    std::unique_ptr<DictSegment> surname_dict_;
    std::unique_ptr<DictSegment> suffix_dict_;
    std::unique_ptr<DictSegment> prep_dict_;

    std::string conf_dir_;
    std::unordered_map<std::string, std::string> props_;

    Dictionary(const std::string &dir);

    std::string GetProperty(const std::string &key) {
        if (props_.find(key) != props_.end()) {
            return props_[key];
        }
        return "";
    }

    Status Load();

    void WalkFileTree(std::vector<std::string> &files, const std::string &path_str);

    Status LoadDictFile(DictSegment *dict, const std::string &file_path, bool critical, const std::string &name);

    std::vector<std::string> GetExtDictionarys();

    std::vector<std::string> GetExtStopWordDictionarys();

    std::string GetDictRoot() { return conf_dir_; }

    void AddWords(const std::vector<std::string> &words);

    void DisableWords(const std::vector<std::string> &words);

    Hit *MatchInMainDict(const std::vector<wchar_t> &char_array);

    Hit *MatchInMainDict(const std::vector<wchar_t> &char_array, int begin, int length);

    Hit *MatchInQuantifierDict(const std::vector<wchar_t> &char_array, int begin, int length);

    Hit *MatchWithHit(const std::vector<wchar_t> &char_array, int current_index, Hit *matched_hit);

    bool IsStopWord(const std::vector<wchar_t> &char_array, int begin, int length);

    Status LoadMainDict();

    Status LoadExtDict();

    Status LoadStopWordDict();

    Status LoadQuantifierDict();

    Status LoadSurnameDict();

    Status LoadSuffixDict();

    Status LoadPrepDict();

private:
    void ParseProperties(const std::string &content);
};
} // namespace infinity