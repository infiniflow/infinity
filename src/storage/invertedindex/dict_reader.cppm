module;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_config;
export module dict_reader;

namespace infinity {

export class DictionaryReader {
public:
    DictionaryReader(const String &root_path){};

    ~DictionaryReader() = default;

    bool Lookup(const String &key, u64 &value) { return false; }
};
} // namespace infinity
