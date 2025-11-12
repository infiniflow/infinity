// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

module infinity_core:physical_read_cache.impl;

import :physical_read_cache;
import :column_binding;
import :infinity_exception;
import :data_block;
import :column_vector;

import std;

import data_type;

namespace infinity {

void PhysicalReadCache::Init(QueryContext *query_context) {}

PhysicalReadCache::PhysicalReadCache(u64 id,
                                     LogicalNodeType origin_type,
                                     std::shared_ptr<BaseTableRef> base_table_ref,
                                     std::shared_ptr<CacheContent> cache_content,
                                     std::vector<size_t> column_map,
                                     std::shared_ptr<std::vector<LoadMeta>> load_metas,
                                     bool is_min_heap)
    : PhysicalOperator(PhysicalOperatorType::kReadCache, nullptr, nullptr, id, load_metas), base_table_ref_(base_table_ref),
      cache_content_(cache_content), column_map_(column_map), is_min_heap_(is_min_heap) {
    switch (origin_type) {
        case LogicalNodeType::kMatch: {
            origin_type_ = PhysicalOperatorType::kMatch;
            break;
        }
        case LogicalNodeType::kKnnScan: {
            origin_type_ = PhysicalOperatorType::kKnnScan;
            break;
        }
        case LogicalNodeType::kMatchSparseScan: {
            origin_type_ = PhysicalOperatorType::kMatchSparseScan;
            break;
        }
        case LogicalNodeType::kMatchTensorScan: {
            origin_type_ = PhysicalOperatorType::kMatchTensorScan;
            break;
        }
        case LogicalNodeType::kIndexScan: {
            origin_type_ = PhysicalOperatorType::kIndexScan;
            break;
        }
        default: {
            UnrecoverableError("Not implemented");
        }
    }
}

bool PhysicalReadCache::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *read_cache_state = static_cast<ReadCacheState *>(operator_state);
    std::vector<std::unique_ptr<DataBlock>> &data_block_array = read_cache_state->data_block_array_;
    for (const std::unique_ptr<DataBlock> &cache_block : cache_content_->data_blocks_) {
        std::vector<std::shared_ptr<ColumnVector>> column_vectors;
        for (size_t i : column_map_) {
            column_vectors.push_back(cache_block->column_vectors_[i]);
        }
        auto data_block = std::make_unique<DataBlock>();
        data_block->Init(std::move(column_vectors));
        data_block_array.emplace_back(std::move(data_block));
    }
    read_cache_state->SetComplete();
    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalReadCache::GetOutputNames() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("MatchCache data blocks is empty");
    }
    auto result_strings = std::make_shared<std::vector<std::string>>();
    for (size_t col_id : column_map_) {
        std::string column_name = cache_content_->column_names_->at(col_id);
        result_strings->push_back(std::move(column_name));
    }
    return result_strings;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalReadCache::GetOutputTypes() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("MatchCache data blocks is empty");
    }
    auto result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    for (size_t i : column_map_) {
        std::shared_ptr<DataType> data_type = cache_content_->data_blocks_[0]->column_vectors_[i]->data_type();
        result_types->push_back(data_type);
    }
    return result_types;
}

} // namespace infinity
