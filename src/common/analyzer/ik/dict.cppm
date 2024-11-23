module;

export module ik_dict;

import stl;
import ik_dict_segment;
import hit;

namespace infinity {
export class Dictionary {
public:
    DictSegment *main_dict_{nullptr};
    DictSegment *quantifier_dict_{nullptr};
    DictSegment *stop_words_{nullptr};

    String conf_dir;
    HashMap<String, String> props;

    Dictionary(const String &dir);

    String GetProperty(const String &key) {
        if (props.find(key) != props.end()) {
            return props[key];
        }
        return "";
    }

    void Initial();

    void WalkFileTree(Vector<String> &files, const String &path_str);

    void LoadDictFile(DictSegment *dict, const String &file_path, bool critical, const String &name);

    Vector<String> GetExtDictionarys();

    Vector<String> GetExtStopWordDictionarys();

    String GetDictRoot() { return conf_dir; }

    void AddWords(const Vector<String> &words);

    void DisableWords(const Vector<String> &words);

    Hit *MatchInMainDict(const Vector<wchar_t> &charArray);

    Hit *MatchInMainDict(const Vector<wchar_t> &charArray, int begin, int length);

    Hit *MatchInQuantifierDict(const Vector<wchar_t> &charArray, int begin, int length);

    Hit *MatchWithHit(const Vector<wchar_t> &charArray, int currentIndex, Hit *matchedHit);

    bool IsStopWord(const Vector<wchar_t> &charArray, int begin, int length);

    void LoadMainDict();

    void LoadExtDict();

    void LoadStopWordDict();

    void LoadQuantifierDict();

    void LoadSurnameDict();

    void LoadSuffixDict();

    void LoadPrepDict();

private:
    void ParseProperties(const String &content);
};
} // namespace infinity