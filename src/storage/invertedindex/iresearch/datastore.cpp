module;

import stl;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import column_vector;

module iresearch_datastore;

namespace infinity {
IResearchDataStore::IResearchDataStore(const String &directory) : directory_(directory) {}

void IResearchDataStore::BatchInsert(SharedPtr<DataBlock> data_block) {
    Vector<SharedPtr<ColumnVector>> column_vectors = data_block->column_vectors;
    for (SizeT i = 0; i < column_vectors.size(); ++i) {
    }
}

const IResearchSubReader *ViewSnapshot::operator[](SizeT i) noexcept { return segments_[i].segment_; }

const ViewSegment &ViewSnapshot::GetSegment(SizeT i) noexcept { return segments_[i]; }

} // namespace infinity