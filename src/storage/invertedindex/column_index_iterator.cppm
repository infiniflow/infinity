module;

export module column_index_iterator;

import stl;

import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import dict_reader;
import local_file_system;
namespace infinity {

export class ColumnIndexIterator {
public:
    ColumnIndexIterator(const String &index_dir, const String &base_name, optionflag_t flag);

    ~ColumnIndexIterator();

    bool Next(String &term, PostingDecoder *&decoder);

private:
    void DecodeDocList();

    void DecodePosList();

    String dict_file_path_;
    String posting_file_path_;
    SharedPtr<DictionaryReader> dict_reader_;
    SharedPtr<FileReader> posting_file_;
    ByteSlice *doc_list_slice_{nullptr};
    ByteSlice *pos_list_slice_{nullptr};

    SharedPtr<ByteSliceReader> doc_list_reader_;
    SharedPtr<ByteSliceReader> pos_list_reader_;

    TermMeta term_meta_;

    SharedPtr<PostingDecoder> posting_decoder_;
};

} // namespace infinity