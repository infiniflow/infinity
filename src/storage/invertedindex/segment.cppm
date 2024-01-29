module;

import stl;
import index_defines;
export module segment;

namespace infinity {

export struct SegmentMeta {
    SegmentMeta() : segment_id_(INVALID_SEGMENTID) {}
    SegmentMeta(segmentid_t segment_id) : segment_id_(segment_id) {}
    ~SegmentMeta() = default;

    void Load() {}

    void Store() {}

    u64 column_id_;
    segmentid_t segment_id_;
    docid_t base_doc_id_;
    u64 doc_count_{0};
    i64 timestamp_;
};

class Indexer;
export class Segment {
public:
    enum SegmentStatus { BUILDING, DUMPLING, BUILT };
    Segment(SegmentStatus segment_status) : segment_status_(segment_status) {}

    ~Segment() = default;

public:
    u64 GetColumnID() const { return segment_meta_.column_id_; }
    segmentid_t GetSegmentID() const { return segment_meta_.segment_id_; }
    docid_t GetBaseDocId() const { return segment_meta_.base_doc_id_; }
    u64 GetDocCount() const { return segment_meta_.doc_count_; }
    void SetSegmentMeta(const SegmentMeta &segment_meta) { segment_meta_ = segment_meta; }
    i64 GetTimestamp() const { return segment_meta_.timestamp_; }
    void SetTimestamp(i64 timestamp) { segment_meta_.timestamp_ = timestamp; }
    SegmentStatus GetSegmentStatus() const { return segment_status_; }
    SharedPtr<Indexer> GetIndexWriter();

private:
    SegmentMeta segment_meta_;
    SegmentStatus segment_status_;
};
} // namespace infinity
