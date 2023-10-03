#pragma once

#include "skiplist_reader.h"
#include "storage/indexstore/index_defines.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {
class TriValueSkipListReader : public SkipListReader {
public:
    TriValueSkipListReader();

    TriValueSkipListReader(const TriValueSkipListReader& other) noexcept;

    ~TriValueSkipListReader();

public:
    virtual void
    Load(const ByteSliceList* byte_slice_list, uint32_t start, uint32_t end, const uint32_t& item_count);

    virtual void
    Load(ByteSlice* byte_slice, uint32_t start, uint32_t end, const uint32_t& item_count);

    bool
    SkipTo(uint32_t query_doc_id, uint32_t& doc_id, uint32_t& prev_doc_id, uint32_t& offset, uint32_t& delta);

    bool
    SkipTo(uint32_t query_doc_id, uint32_t& doc_id, uint32_t& offset, uint32_t& delta) {
        return SkipTo(query_doc_id, doc_id, prev_doc_id_, offset, delta);
    }

    uint32_t
    GetPrevDocId() const {
        return prev_doc_id_;
    }

    uint32_t
    GetCurrentDocId() const {
        return current_doc_id_;
    }

    uint32_t
    GetCurrentTTF() const {
        return current_ttf_;
    }

    uint32_t
    GetPrevTTF() const {
        return prev_ttf_;
    }

    virtual uint32_t
    GetLastValueInBuffer() const {
        return 0;
    }

    virtual uint32_t
    GetLastKeyInBuffer() const {
        return 0;
    }

protected:
    void
    InitMember();

    virtual std::pair<int, bool>
    LoadBuffer();

private:
    void
    Load_(uint32_t start, uint32_t end, const uint32_t& item_count);

protected:
    uint32_t current_doc_id_;
    uint32_t current_offset_;
    uint32_t current_ttf_;
    uint32_t prev_doc_id_;
    uint32_t prev_offset_;
    uint32_t prev_ttf_;
    uint32_t doc_id_buffer_[SKIP_LIST_BUFFER_SIZE];
    uint32_t offset_buffer_[SKIP_LIST_BUFFER_SIZE];
    uint32_t ttf_buffer_[SKIP_LIST_BUFFER_SIZE];
    uint32_t current_cursor_;
    uint32_t num_in_buffer_;
};

}// namespace infinity