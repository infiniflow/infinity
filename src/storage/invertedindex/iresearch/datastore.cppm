module;

import stl;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import column_vector;

export module iresearch_datastore;

namespace infinity {

export struct ViewSegment {
    ViewSegment(IResearchSubReader &segment) : segment_(&segment) {}
    IResearchSubReader *segment_;
};

export class IResearchDataStore {
public:
    IResearchDataStore(const String &directory);

    void BatchInsert(SharedPtr<DataBlock> data_block);

private:
    String directory_;
};

using DataSnapshotPtr = SharedPtr<IResearchDataStore>;

export class ViewSnapshot : public IResearchIndexReader {
public:
    ViewSnapshot() noexcept = default;

    const IResearchSubReader *operator[](SizeT i) noexcept;

    const ViewSegment &GetSegment(SizeT i) noexcept;

public:
    Vector<ViewSegment> segments_;
    DataSnapshotPtr data_store_;
};

} // namespace infinity
