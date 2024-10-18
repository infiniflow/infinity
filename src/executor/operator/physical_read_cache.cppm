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

export module physical_read_cache;

import stl;
import physical_operator;
import query_context;
import operator_state;
import base_table_ref;
import data_type;
import result_cache_manager;
import physical_operator_type;
import load_meta;
import logical_node_type;

namespace infinity {

export class PhysicalReadCache : public PhysicalOperator {
public:
    PhysicalReadCache(u64 id,
                      LogicalNodeType origin_type,
                      SharedPtr<BaseTableRef> base_table_ref,
                      SharedPtr<CacheContent> cache_content,
                      Vector<SizeT> column_map,
                      SharedPtr<Vector<LoadMeta>> load_metas);

    void Init() override {};

    virtual SizeT TaskletCount() override { return 1; }

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;

    const BaseTableRef *base_table_ref() const { return base_table_ref_.get(); }

    PhysicalOperatorType origin_type() const { return origin_type_; }

private:
    SharedPtr<BaseTableRef> base_table_ref_;

    SharedPtr<CacheContent> cache_content_;

    PhysicalOperatorType origin_type_;
    Vector<SizeT> column_map_; // result column id -> cache column id
};

} // namespace infinity
