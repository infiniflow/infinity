module;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import index_config;
import segment;
module disk_index_segment_reader;

namespace infinity {
DiskIndexSegmentReader::DiskIndexSegmentReader(const String &root_path, const Segment &segment) : path_(root_path) {}
} // namespace infinity
