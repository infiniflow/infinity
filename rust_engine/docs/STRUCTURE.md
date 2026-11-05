# 目录结构对照

> 子目标 S8 · 目录与文件 basename 尽量对齐参考版｜索引见 [`PORTING.md`](PORTING.md) §3.2

目标：读 Rust 代码时能按参考版的路径找东西。规则是**crate ≈ 参考版 `src/` 下的一级目录**，crate 内部再按参考版的一到两层目录分文件；文件 basename 尽量与参考版对应文件同名。

crate 不带 `infinity-` 前缀：`crates/storage` 对应 `src/storage`，`crates/executor` 对应 `src/executor`。唯一要注意的是 `common` 这个 crate 名与 `storage`/`executor` 内部的 `common` 模块同名，那两个 crate 里引用外部 crate 时写绝对路径（`::common::…`）。

## 1 crate 与参考版一级目录

| 参考 | 重写版 crate | 说明 |
|---|---|---|
| `src/common` | `common` | 基础类型、JSON、SIMD、utility、datetime、varchar |
| `src/storage` | `storage` | 见 §2 |
| `src/storage/catalog` + catalog 部分的 `new_txn` | `catalog` | catalog 的键空间与值编码单独成 crate（参考版在 storage 里） |
| `src/storage/new_txn` 的事务部分 | `txn` | |
| `src/parser` | `parser` | |
| `src/expression` + `src/function` | `expression` | 标量/聚合函数与表达式求值 |
| `src/planner` | `planner` | |
| `src/storage/knn_index` | `index` | |
| `src/executor` + `src/scheduler` + `src/admin` | `executor` | |
| `src/network` | `server` | |
| `src/main` | `cli` | |

## 2 `storage` 内部

| 参考 | 重写版 |
|---|---|
| `storage/invertedindex/format/*` | `src/invertedindex/format/{mod,doc_list,position_list,posting_slice,skiplist}.rs` |
| `storage/invertedindex/disk_segment_reader`、`posting_iterator`、`posting_writer` | `src/invertedindex/disk_segment_reader.rs` |
| `storage/invertedindex/common/{external_sort_merger,loser_tree}` | `src/invertedindex/common/external_sort.rs`（loser tree 在 `common/src/loser_tree.rs`） |
| `storage/invertedindex/fst/registry` | `src/invertedindex/fst/registry.rs` |
| `storage/buffer/{buffer_manager,buffer_obj}` | `src/buffer/{mapped,mapped_block}.rs`（没有 BufferManager，见 PORTING §3.1） |
| `storage/column_vector/{selection,value}` | `src/column_vector/selection.rs` |
| `storage/common/{block_index,crc,rcu_multimap}` | `src/common/{block_index,crc,rcu_map}.rs` |
| `storage/definition/*` | `src/definition/cpp_format.rs` |
| `storage/fast_rough_filter/binary_fuse_filter` | `src/fast_rough_filter/binary_fuse_filter.rs` |
| `storage/io/{virtual_store,s3_client,byte_slice_*}` | `src/io/{mod,s3,virtual_store}.rs` |
| `storage/knn_index/emvb` | `src/knn_index/emvb/emvb_segment.rs` |
| `storage/knn_index/multivector` | `src/knn_index/multivector/multivector_segment.rs` |
| `storage/knn_index/sparse/bmp_*` | `src/knn_index/sparse/{bmp,bmp_segment}.rs` |
| `storage/knn_index/{knn_distance,merge_knn}` | `src/knn_index/knn_format.rs` |
| `storage/new_txn/{txn_kv,mvcc}` | `src/new_txn/{txn_kv,mvcc}.rs` |
| `storage/persistence/persistence_manager` | `src/persistence/persistence_manager.rs` |
| `storage/secondary_index/*` | `src/secondary_index/{secondary_index_data,secondary_lowcard,secondary_segment_pgm}.rs` |
| `storage/wal/*` | `src/wal/{mod,wal_entry,wal_manager,block,bottom_executor,ddl_def,index_def,log_file,wal_cmd}.rs` |
| `storage/bg_task/mem_index_appender` | `src/bg_task/memindex_tracer.rs` |
| `storage/{data_block,result_cache_manager}` | `src/{data_block,result_cache_manager}.rs` |
| `storage/catalog/{kv_store,meta_cache,new_catalog}` | `src/catalog/{catalog_format,metastore,metastore_rocks}.rs` |

## 3 `executor` 内部

| 参考 | 重写版 |
|---|---|
| `executor/operator/*` | `src/operator/{mod,scan,filter,project,aggregate,sort,top,limit,lower}.rs` |
| `storage/invertedindex/search/blockmax_wand_iterator` + `bm25_ranker` | `src/invertedindex/search/blockmax_wand_iterator.rs` |
| `storage/invertedindex/search/query_builder` | `src/invertedindex/search/query_builder.rs` |
| `storage/invertedindex/search/highlighter` | `src/lib.rs` 的 `highlight_text` / `highlight_with_analyzer` |
| `common/analyzer/*` | `src/common/analyzer.rs`（单个文件，按分析器分节） |
| `executor/explain_*` | `src/{explain,explain_search}.rs` |
| `scheduler/*` | `src/scheduler.rs` |

## 4 有意不一致的地方

1. **`executor/src/lib.rs` 仍是单文件**（约 30k 行）。参考版把这些拆成 `physical_insert/physical_select/…` 与 `fragment/*`。端口把执行流集中在一个文件里的 `impl Engine`，因为方法之间共享大量私有状态；按参考版切分要把每个方法改成 `pub(crate)` 并跨模块引用，属于纯机械但高风险的改动，**尚未做**，登记在 `GAPS.md` §2。
2. **测试文件与实现同目录**（`*_ut.rs`），参考版在 `src/unit_test/<子目录>`。移动测试要改 `use super::*` 的层级，收益低于风险，未做。
3. **catalog 与 new_txn 拆成独立 crate**（参考版都在 `storage` 里）。这是 crate 边界的设计变更，见 `PORTING.md` §3.1。
4. **`cpp_blocks.rs`（块布局 + 读写）留在 executor**：它同时依赖 storage 的块格式与 executor 的表状态，放在任一 crate 都要反向依赖。参考版对应 `storage/buffer/file_worker/*` 与 `column_vector/*`。
5. **扁平别名仍在**：每个 crate 的 `lib.rs` 保留了旧路径的 `pub use`（如 `storage::catalog_format`），所以外部调用方没有因为这次搬家而改动。别名是过渡层，新代码应直接用层级路径。
