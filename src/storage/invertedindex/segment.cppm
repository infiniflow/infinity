module;

import stl;
import index_defines;
import file_reader;
import file_writer;

export module segment;

namespace infinity {

// Segment is a logical concept for indices overall all columns
// We need to let indices of all columns flushed and merged together
export struct SegmentMeta {
    SegmentMeta() : segment_id_(INVALID_SEGMENTID) {}
    SegmentMeta(segmentid_t segment_id) : segment_id_(segment_id) {}
    ~SegmentMeta() = default;

    void Load(const SharedPtr<FileReader> &reader) {
        segment_id_ = (u32)reader->ReadVInt();
        base_doc_id_ = (u32)reader->ReadVInt();
        doc_count_ = (u32)reader->ReadVInt();
        timestamp_ = (u64)reader->ReadVLong();
    }

    void Store(const SharedPtr<FileWriter> &file) {
        file->WriteVInt(segment_id_);
        file->WriteVInt(base_doc_id_);
        file->WriteVInt(doc_count_);
        file->WriteVLong(timestamp_);
    }

    segmentid_t segment_id_{INVALID_SEGMENTID};
    docid_t base_doc_id_{INVALID_DOCID};
    u64 doc_count_{0};
    i64 timestamp_;
};

class Indexer;
export class Segment {
public:
    enum SegmentStatus {
        BUILDING, // In memory segment
        DUMPLING, // Flush
        BUILT     // Disk segment
    };
    Segment(SegmentStatus segment_status) : segment_status_(segment_status) {}

    ~Segment() = default;

public:
    static constexpr segmentid_t MERGED_SEGMENT_ID_MASK = (segmentid_t)0x0;

    segmentid_t GetSegmentId() const { return segment_meta_.segment_id_; }
    void SetSegmentId(segmentid_t segment_id) { segment_meta_.segment_id_ = segment_id; }
    bool IsMergedSegmentId(segmentid_t segment_id) { return segment_id != INVALID_SEGMENTID && (segment_id & MERGED_SEGMENT_ID_MASK) == 0; }
    docid_t GetBaseDocId() const { return segment_meta_.base_doc_id_; }
    void SetBaseDocId(docid_t base_doc) { segment_meta_.base_doc_id_ = base_doc; }
    void IncDocCount(u64 doc_count) { segment_meta_.doc_count_ += doc_count; }
    void SetDocCount(u64 doc_count) { segment_meta_.doc_count_ = doc_count; }
    void SetSegmentMeta(const SegmentMeta &segment_meta) { segment_meta_ = segment_meta; }
    i64 GetTimestamp() const { return segment_meta_.timestamp_; }
    void SetTimestamp(i64 timestamp) { segment_meta_.timestamp_ = timestamp; }
    SegmentStatus GetSegmentStatus() const { return segment_status_; }
    void SetSegmentStatus(SegmentStatus status) { segment_status_ = status; }
    Indexer *GetIndexWriter() const { return indexer_; }
    void SetIndexWriter(Indexer *indexer) { indexer_ = indexer; }

private:
    SegmentMeta segment_meta_;
    SegmentStatus segment_status_;
    Indexer *indexer_{nullptr};
};

export class IDGenerator {
public:
    IDGenerator() = default;
    ~IDGenerator() = default;
    IDGenerator(const IDGenerator &) = delete;
    IDGenerator(const IDGenerator &&) = delete;

    segmentid_t GetNextSegmentID() {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_segment_id_ + 1;
    }

    void SetCurrentSegmentID(segmentid_t segment_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        current_segment_id_ = segment_id;
    }

private:
    std::mutex mutex_;
    segmentid_t current_segment_id_{INVALID_SEGMENTID};
};
} // namespace infinity
