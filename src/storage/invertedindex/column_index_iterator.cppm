export module infinity_core:column_index_iterator;

import :byte_slice;
import :byte_slice_reader;
import :file_reader;
import :posting_decoder;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :dict_reader;

namespace infinity {

export class ColumnIndexIterator {
public:
    ColumnIndexIterator(const std::string &index_dir, const std::string &base_name, optionflag_t flag);

    ~ColumnIndexIterator();

    bool Next(std::string &term, PostingDecoder *&decoder);

private:
    void DecodeDocList();

    void DecodePosList();

    std::string dict_file_path_;
    std::string posting_file_path_;
    std::shared_ptr<DictionaryReader> dict_reader_;
    std::shared_ptr<FileReader> posting_file_;
    ByteSlice *doc_list_slice_{nullptr};
    ByteSlice *pos_list_slice_{nullptr};

    std::shared_ptr<ByteSliceReader> doc_list_reader_;
    std::shared_ptr<ByteSliceReader> pos_list_reader_;

    TermMeta term_meta_;

    std::shared_ptr<PostingDecoder> posting_decoder_;
};

} // namespace infinity