module;

export module ik_dict;

import stl;
import ik_dict_segment;
import hit;
import status;

namespace infinity {
export class Dictionary {
public:
    UniquePtr<DictSegment> main_dict_;
    UniquePtr<DictSegment> quantifier_dict_;
    UniquePtr<DictSegment> stop_words_;
    UniquePtr<DictSegment> surname_dict_;
    UniquePtr<DictSegment> suffix_dict_;
    UniquePtr<DictSegment> prep_dict_;

    String conf_dir_;
    HashMap<String, String> props_;

    Dictionary(const String &dir);

    String GetProperty(const String &key) {
        if (props_.find(key) != props_.end()) {
            return props_[key];
        }
        return "";
    }

    Status Load();

    void WalkFileTree(Vector<String> &files, const String &path_str);

    Status LoadDictFile(DictSegment *dict, const String &file_path, bool critical, const String &name);

    Vector<String> GetExtDictionarys();

    Vector<String> GetExtStopWordDictionarys();

    String GetDictRoot() { return conf_dir_; }

    void AddWords(const Vector<String> &words);

    void DisableWords(const Vector<String> &words);

    Hit *MatchInMainDict(const Vector<wchar_t> &char_array);

    Hit *MatchInMainDict(const Vector<wchar_t> &char_array, int begin, int length);

    Hit *MatchInQuantifierDict(const Vector<wchar_t> &char_array, int begin, int length);

    Hit *MatchWithHit(const Vector<wchar_t> &char_array, int current_index, Hit *matched_hit);

    bool IsStopWord(const Vector<wchar_t> &char_array, int begin, int length);

    Status LoadMainDict();

    Status LoadExtDict();

    Status LoadStopWordDict();

    Status LoadQuantifierDict();

    Status LoadSurnameDict();

    Status LoadSuffixDict();

    Status LoadPrepDict();

private:
    void ParseProperties(const String &content);
};
} // namespace infinity