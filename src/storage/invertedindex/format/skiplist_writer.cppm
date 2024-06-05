module;

import stl;
import file_writer;
import file_reader;
import posting_byte_slice;
import short_list_optimize_util;

export module skiplist_writer;

namespace infinity {

export class SkipListWriter : public PostingByteSlice {
public:
    SkipListWriter();
    virtual ~SkipListWriter() = default;

    void AddItem(u32 delta_value1);

    void AddItem(u32 key, u32 value1);

    void AddItem(u32 key, u32 value1, u32 value2);

    void AddItem(u32 last_doc_id, u32 total_tf, u32 block_max_tf, u16 block_max_percentage, u32 item_size);

    void Dump(const SharedPtr<FileWriter> &file, bool spill = false);

    void Load(const SharedPtr<FileReader> &file);

private:
    static const u32 INVALID_LAST_KEY = (u32)-1;

    u32 last_key_;
    u32 last_value1_;
};

} // namespace infinity
