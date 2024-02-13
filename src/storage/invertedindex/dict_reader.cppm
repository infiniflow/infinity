module;

import stl;
import term_meta;
import posting_list_format;
import fst;
export module dict_reader;

namespace infinity {

export class DictionaryIterator {
public:
    DictionaryIterator(const String &term) : current_(term) {}

    ~DictionaryIterator() = default;

    bool Next() { return false; }

    void GetCurrentTerm(String &term) const {}

    void GetCurrentTermMeta(TermMeta &term_meta) const {}

    void Seek(String &key) {}

private:
    String current_;
};

export class DictionaryReader {
private:
    const String &dict_path_;
    TermMetaLoader meta_loader_;
    u8 *data_ptr_;
    SizeT data_len_;
    UniquePtr<Fst> fst_;

public:
    DictionaryReader(const String &dict_path, const PostingFormatOption &option);

    ~DictionaryReader();

    bool Lookup(const String &key, TermMeta &term_meta);

    void LookupPrefix(const String &prefix, Vector<Pair<String, TermMeta>> &term_metas);

    SharedPtr<DictionaryIterator> CreateIterator() const;
};
} // namespace infinity
