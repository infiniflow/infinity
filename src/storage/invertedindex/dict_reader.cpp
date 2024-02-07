module;

import stl;
import term_meta;
import posting_list_format;
import fst;
import mmap;
import infinity_exception;
module dict_reader;

namespace infinity {

DictionaryReader::DictionaryReader(const String &dict_path, const PostingFormatOption &option)
    : dict_path_(dict_path), meta_loader_(option), data_ptr_(nullptr), data_len_(0) {
    int rc = MmapFile(dict_path, data_ptr_, data_len_);
    if (rc < 0) {
        throw UnrecoverableException("MmapFile failed");
    }
    // fst_root_addr + addr_offset(21) == fst_len
    SizeT fst_root_addr = ReadU64LE(data_ptr_ + data_len_ - 4 - 8);
    SizeT fst_len = fst_root_addr + 21;
    u8 *fst_data = data_ptr_ + (data_len_ - fst_len);
    fst_ = MakeUnique<Fst>(fst_data, fst_len);
}

DictionaryReader::~DictionaryReader() {
    if (data_ptr_ != nullptr) {
        int rc = MunmapFile(data_ptr_, data_len_);
        if (rc < 0) {
            throw UnrecoverableException("MunmapFile failed");
        }
    }
}

bool DictionaryReader::Lookup(const String &key, TermMeta &term_meta) {
    u64 val;
    bool found = fst_->Get((u8 *)key.c_str(), key.length(), val);
    if (!found)
        return false;
    u8 *data_cursor = data_ptr_ + val;
    SizeT left_size = data_len_ - val;
    meta_loader_.Load(data_cursor, left_size, term_meta);
    return true;
}

void DictionaryReader::LookupPrefix(const String &prefix, Vector<Pair<String, TermMeta>> &term_metas) {
    FstStream s(*fst_, (u8 *)prefix.c_str(), prefix.length());
    Vector<u8> key;
    u64 val;
    String term;
    TermMeta term_meta;
    while (s.Next(key, val)) {
        term = String((char *)key.data(), key.size());
        u8 *data_cursor = data_ptr_ + val;
        SizeT left_size = data_len_ - val;
        meta_loader_.Load(data_cursor, left_size, term_meta);
        term_metas.push_back({term, term_meta});
    }
}

} // namespace infinity