module;

import stl;
import term_meta;
import posting_list_format;
import fst;
export module dict_reader;

namespace infinity {

export class DictionaryReader {
private:
    const String &dict_path_;
    TermMetaLoader meta_loader_;
    u8 *data_ptr_;
    SizeT data_len_;
    UniquePtr<Fst> fst_;
    UniquePtr<FstStream> s_;

public:
    DictionaryReader(const String &dict_path, const PostingFormatOption &option);

    ~DictionaryReader();

    bool Lookup(const String &key, TermMeta &term_meta);

    void InitIterator(const String &min, const String &max);

    void InitIterator(const String &prefix);

    bool Next(String &term, TermMeta &term_meta);
};
} // namespace infinity
