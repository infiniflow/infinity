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

#include <vector>

module physical_read_cache;

import column_binding;
import infinity_exception;
import data_block;
import column_vector;

namespace infinity {

bool PhysicalReadCache::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *read_cache_state = static_cast<ReadCacheState *>(operator_state);
    Vector<UniquePtr<DataBlock>> &data_block_array = read_cache_state->data_block_array_;
    for (const UniquePtr<DataBlock> &cache_block : cache_content_->data_blocks_) {
        Vector<SharedPtr<ColumnVector>> column_vectors;
        for (SizeT i : column_map_) {
            column_vectors.push_back(cache_block->column_vectors[i]);
        }
        auto data_block = MakeUnique<DataBlock>();
        data_block->Init(std::move(column_vectors));
        data_block_array.emplace_back(std::move(data_block));
    }
    read_cache_state->SetComplete();
    return true;
}

SharedPtr<Vector<String>> PhysicalReadCache::GetOutputNames() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("MatchCache data blocks is empty");
    }
    auto result_strings = MakeShared<Vector<String>>();
    for (SizeT col_id : column_map_) {
        String column_name = cache_content_->column_names_->at(col_id);
        result_strings->push_back(std::move(column_name));
    }
    return result_strings;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalReadCache::GetOutputTypes() const {
    if (cache_content_->data_blocks_.empty()) {
        UnrecoverableError("MatchCache data blocks is empty");
    }
    auto result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    for (SizeT i : column_map_) {
        SharedPtr<DataType> data_type = cache_content_->data_blocks_[0]->column_vectors[i]->data_type();
        result_types->push_back(data_type);
    }
    return result_types;
}

} // namespace infinity
