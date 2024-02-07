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

public:
    DictionaryReader(const String &dict_path, const PostingFormatOption &option);

    ~DictionaryReader();

    bool Lookup(const String &key, TermMeta &term_meta);

    void LookupPrefix(const String &prefix, Vector<Pair<String, TermMeta>> &term_metas);
};
} // namespace infinity
