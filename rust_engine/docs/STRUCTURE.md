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
| `storage/definition/*` | `src/definition/definition/format.rs` |
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
| `common/analyzer/*` | `src/common/analyzer/`（单个文件，按分析器分节） |
| `executor/explain_*` | `src/{explain,explain_search}.rs` |
| `scheduler/*` | `src/scheduler.rs` |

## 4 `executor/src/engine/`

原来的 `lib.rs` 是 30k 行单文件；现在 `lib.rs` 只剩模块声明与再导出（135 行），引擎本身按**语句路径**分文件：

| 文件 | 内容 | 行数 |
|---|---|---|
| `engine/mod.rs` | 模块根：导入、再导出 | ~70 |
| `engine/state.rs` | `Engine`/`Storage`/`TableEntry` 等状态类型与它们的方法 | 7.1k |
| `engine/statements.rs` | 语句主路径上的 `impl Engine` 方法（扫描、执行、子查询） | 7.8k |
| `engine/query.rs` | 查询解析/打分辅助（`query_units`、字段查询、tokenize/highlight） | 2.6k |
| `engine/dml.rs` | insert / append / delete / update / copy / import | 2.1k |
| `engine/index_build.rs` | 建索引、dump、seal、reload | 1.4k |
| `engine/text_index.rs` | 内存索引器、`TermPostings`、`TextIndex` 等 | 1.2k |
| `engine/types.rs` | 类型文本解析、fp16/bf16、张量/稀疏判定 | 0.9k |
| `engine/admin.rs` | show / cleanup / optimize / compact | 0.6k |
| `engine/ddl.rs` | create / drop / alter | 0.4k |
| `engine/tasks.rs` | 三个后台任务 | 0.1k |
| `engine/tests_ut.rs` | 原来的 `mod tests`（原样搬出） | 5.6k |

拆法说明：`Engine` 的定义和它的字段留在 `state.rs`，其余文件各自写 `impl Engine { … }`；因为都在 `engine` 这棵子树里，私有字段和私有方法天然可见，只需要把跨文件调用的方法和字段放宽到 `pub(crate)`。语句路径之外还有 8 个文件、约 1 万行的辅助函数，按名字前缀归到上面的桶里；`statements.rs` 里剩下的 191 个方法就是引擎的核心语句流程，彼此交叉引用密集，没有再往下切。

## 5 有意不一致的地方

1. **`engine/` 的切法按语句路径，不是参考版的 `physical_*.cpp` 一对一**：参考版一个物理算子一个文件，端口的 `impl Engine` 是单体实现（一个 `execute` 分派到底），拆成一对一需要先重构成算子对象，属于行为改动而不是整理，未做。
2. **测试文件与实现同目录**（`*_ut.rs`），参考版在 `src/unit_test/<子目录>`。移动测试要改 `use super::*` 的层级，收益低于风险，未做。
3. **catalog 与 new_txn 拆成独立 crate**（参考版都在 `storage` 里）。这是 crate 边界的设计变更，见 `PORTING.md` §3.1。
4. **`blocks/`（块布局 + 读写）留在 executor**：它同时依赖 storage 的块格式与 executor 的表状态，放在任一 crate 都要反向依赖。参考版对应 `storage/buffer/file_worker/*` 与 `column_vector/*`。
5. **扁平别名仍在**：每个 crate 的 `lib.rs` 保留了旧路径的 `pub use`（如 `storage::catalog_format`），所以外部调用方没有因为这次搬家而改动。别名是过渡层，新代码应直接用层级路径。

## 6 还没拆的大文件

按行数排列（`wc -l`）。前三项是下一步的候选，切法与 `engine/` 相同：按参考版的目录/文件分，共享状态留在父模块。

| 文件 | 行数 | 参考版对应 | 建议切法 |
|---|---|---|---|
| `engine/statements.rs` / `engine/state.rs` | 7.8k / 7.1k | `executor/physical_*.cpp` | 需要先把 `execute` 的单体分派拆成算子对象，属行为改动 |
| `server/src/lib.rs` | 8.7k | `network/*`、`network/http/*` | `network/{server, session, connection, thrift}`，HTTP 与 PG 各自成文件（`http.rs`/`pg.rs`/`peer.rs` 已存在） |
| `engine/tests_ut.rs` | 5.6k | `unit_test/**` | 按被测主题再分若干文件 |
| `blocks/engine.rs` | 5.6k | `storage/buffer/file_worker/*` | 与上面两条同理：`impl Engine` 的块读写方法，再切要按方法族拆 |
| `parser/src/parser.rs` | 4.2k | `parser/{statement, expr, type, table_reference}` | 按语句类型切 |
| `server/src/peer.rs` / `http.rs` | 3.6k / 3.2k | `network/peer_server_thrift`、`network/http` | 按 handler 切 |
| `catalog/src/layout.rs` | 3.3k | `storage/catalog/meta/*` | `catalog/meta/{key, value, tree}` |
| `planner/src/index_scan.rs` | 3.2k | `planner/optimizer/index_scan_builder` + `function/table/*` | 建索引扫描算子与各索引类型的候选生成分开 |
| `storage/wal/wal_entry.rs` | 3.1k | `storage/wal/wal_entry*.cppm` | 按 `WalCmd` 变体族切 |
| `storage/data_block.rs` | 2.9k | `storage/data_block.cppm` | 与参考版一致，单文件即可 |
