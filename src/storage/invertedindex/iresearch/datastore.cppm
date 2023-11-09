module;

import stl;
import iresearch_document;
import iresearch_analyzer;
import data_block;
import column_vector;

export module iresearch_datastore;

namespace infinity {

export class IResearchDataStore {
public:
    IResearchDataStore(const String &directory);

    void BatchInsert(SharedPtr<DataBlock> data_block);

private:
    String directory_;
};
} // namespace infinity
