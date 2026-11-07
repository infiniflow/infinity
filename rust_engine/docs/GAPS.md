# 现状缺口清单

> **子目标 S7 · 缺口收口**｜纲领见 [`PORTING.md`](PORTING.md) §5

本文件是**唯一**的缺口汇总：哪些地方与参考不同、为什么、影响是什么。分四类，另有已登记的行为差异（§5，不是待办）：

- **§1 有意不移植**：已经决定不做，不是待办；
- **§2 未实现 / 未接线**：出现时附证据与影响；
- **§3 不可逐字节比对**：参考自身不确定或依赖实现细节，门禁只能按内容比；
- **§4 覆盖不到的输入空间**：机制有了，门禁还没跑到那个规模/形态。

格式层面的逐文件细节在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5；逐类/逐用例的映射状态在
[`CLASS_MAP.md`](CLASS_MAP.md) §2（`mapped` / `partial` / `excluded` 行）与 [`TEST_AUDIT.md`](TEST_AUDIT.md)（`excluded` 行，对应本文 §1 的不移植决定）。
**已接受的设计变更**（不是待办）登记在 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」；与持久化边界有关的那一条同时记在本文 §1.1，因为它是一个「有意不移植」的决定。

---

## 1 有意不移植

| 项 | 决定 | 影响 | 依据 |
|---|---|---|---|
| 远程对象存储（`obj_key` 打包 + VirtualStore 上传、catalog 的 `meta/` 路径树同步） | 不移植；本地对象目录（`pm\|object\|` 记录 + 打包对象）仍按参考布局写 | 桶内布局与参考不同，跨引擎"同一桶互读"不成立 | `FORMAT_EQUIVALENCE_TESTS.md` §6.5 |
| DiskANN | 不移植 | 该索引类型不可用 | 计划书 §2.6/R14 |
| geo 类型（`Point`/`Line`/`LineSeg`/`Box`/`Circle`） | 不移植；`GeographyTryCastToVarlen` 随 cast 保留 | 这些列类型不可用 | `TEST_AUDIT.md` 的 `excluded` 行 |

### 1.1 mem index 的 flush 持久化（有意的设计偏离）

**重写版在 flush 时不持久化任何一类 mem index，重启时在 `recover_mem_index` 里恢复**。checkpoint（`flush_storage`）只写块，
**不**封存任何一类内存索引（secondary / fulltext / HNSW / IVF / BMP / 多值）。索引尾巴只由 `DumpMemIndex` 事务封存：
它先按 kind 把尾巴写成 chunk/段文件，再在同一个事务里写 `idx_chunk|…` 记录（值 = 该事务的 commit ts）；尾巴为空且没有
未发布的 chunk 文件时直接返回，不花 commit ts、不写 WAL entry（对应参考的 "skipped dumping MemIndex"）。

重启后 `Engine::open` 末尾调用 `recover_mem_index`：用 catalog 的 chunk 覆盖（`base_row_id`/`row_count` 合并成
升序区间）与块内行求差，只把没被 chunk 覆盖的行喂回内存索引。各 kind 的恢复路径：HNSW / IVF 直接插回内存段；fulltext
走 `text_index_for`（`load_sealed` 映射已有 chunk，再从第一个未覆盖行重建）；二级索引由 `rebuild_indexes` 的
`build_index` 全量重建；多值与稀疏没有独立的 chunk 映射，在首次查询时整表重建。

**影响**：flush 之后索引尾巴仍在内存里（只由配额、按批容量或显式 `DumpIndex` 落盘），崩溃不丢行（行在块和 WAL 里），
代价是重启时多做一次 gap 重建。checkpoint 的索引镜像（`flush_blocks`）会写 chunk 文件却没有 ts 可写，这类「有文件、
无记录」的 chunk 由下一次 dump 赋 ts（`dump_index_transaction` 的 `index_has_unpublished_chunks` 判据）。
门禁：`test_cross_engine_bytes_20_table`（各索引 chunk 数）、`_30_segments`（配额驱动的 dump 与 tiling）、
`_45_ddl`（9 阶段，含 create/drop index）、`_50_mutations`（重启/崩溃后的索引覆盖）。

## 2 未实现 / 未接线

**本节没有未接线项。** 这一层的现状：

- **行以列式块为存储本体**（方案 C）：`INSERT` 的行只进 `TableEntry::pending_rows`（`BufferObject` 的角色），块文件由 checkpoint（`flush_blocks` → `write_blocks`）写，`COPY` 例外（见下）。KV 里**没有任何行键**：`row:` 既不写也不读，`del:` 只剩事务命令流里的名字（`wal_delete_key`，用于构造 `WalCmdDeleteV2` 与行级冲突判定），不落盘；行侧车（`persistence/rowdata_*.bin`）与 `persistence/` 里的行记录一并去掉（该目录只剩参考版形状的空目录）。读全部把 buffer 盖在块上：整表 / 剪枝 / 列投影 / 点取 / 含墓碑 / 计数 / `row_is_present`；回放把 `WalCmd::Append`/`Delete` 交回 buffer（已落盘的 id 跳过），IMPORT 先写块再写 WAL 帧。
- **封存块与索引段都走 mmap，参考的 `BufferManager` 不移植**：`MappedBlock`（`storage::buffer::mapped_block`）把块的每个 `{col}.col` 映射进来，列体是映射里的切片，槽位解码直接读 page cache；索引段由 `mapped::MappedFile` 映射。映射在进程内共享：`Context::block_maps` 每个 `BlockId` 只留一个 `Arc<MappedBlock>`，块被重写/压缩/删除/表被 drop 时按表丢弃（`drop_block_maps`）。未封存的行本来就在 `TableEntry::pending_rows`（每表一份，所有读者共享）。`BufferObj`/`BufferHandle`/`BufferPool`/`FileWorker`/`VarBuffer` 不再存在；`[buffer] buffer_manager_size` 只剩索引缓冲的预算，`SHOW BUFFERS` 一行 = 一个映射。
- **cleanup 的活段判据**是 `visible_block_ids()`（块索引，跨重启有效），不是 `segment_ids()`（分配器，重开目录后为空；用它会把每个 `idx_*/seg_{n}` 当死段删掉）。
- **行号的次序**：冲突检查之后、写 WAL 之前按 `commit_ts` 分配（`TxnManager::begin_write` / `commit_at_deferred_with`，对应参考的 `wait_conflict_ck_` / `SubmitForAllocation`），checkpoint 的上界是 `current_ts`（已完成的 commit_ts），块的窗口按 `append_totals` + `append_ts` 裁到 `ts <= 上界`，块内 id 必须是窗口的稠密前缀。
- **多值 / 张量表的读路径**：这类表的解码文本形式在写入与扫描两条路上不稳定，`wide_text_columns` 只用来关掉「直接读 buffer 的 `Value`」快路径，改走块解码；不再保留文本行副本。
- **块格式表达不了的列类型**（DDL 拒绝之外的类型）：没有块可写，也没有行日志可写，这类表的行只活在 append buffer 里（进程内可见，重启即失）。现有 DDL 支持的列类型全部有块布局，所以这条路径目前不可达。
- **列投影的并发守卫**：块文件就是行集，不需要「本进程写过镜像」的快照，快照只做并发守卫（`try_projection`）。

崩溃恢复的口径：**compaction 在自己的提交里写它合并出的段**（`write_blocks_bounded`，块与 `catalog|seg|…` 记录一起落盘），否则崩溃时 `drop|seg|…` 标记已经 durable、新段却还没有块，表会读成空；**WAL 回放除表名外还比表 id**（`ReplayedAppend::table_id`），快照恢复把表换成了新 id，旧 id 的 append 帧不会再叠到恢复出来的行上；**checkpoint 的 segment 清理保留被提交上界裁掉的段**，否则一个先于某次提交开始的 checkpoint 会删掉那次提交刚写好的块目录。

回归覆盖：`tests::concurrent_appends_survive_a_checkpoint_and_reopen`、`blocks::tests::a_checkpoint_writes_only_the_committed_prefix`、`tests::a_multivector_table_round_trips_through_its_blocks`、门禁 30 / 40。当前规模：executor 704 passed / 29 ignored、catalog 54、storage 464 passed / 5 ignored、txn 30，全绿。


**导入在自己的提交里封段、落块**：`commit_import_chunk` 提交后按段容量（8,388,608 行）回绕、记录导入 extent（它决定段是否「封存」，也就是块和段要不要写 fast rough filter），再 `write_blocks_bounded` 把这一批行写进块；`copy_from` 的 `FORMAT FVECS` 分支同样。所以 `COPY` 结束就能看到磁盘上的真实分块：8,388,608 行 → 1 段 1024 块，8,388,609 行 → 2 段（1024 + 1 块）。

**DDL 是增量写，不重写整份目录**：`create_database` / `drop_database` 只写库级记录；`create_table` 走 `stage_new_table`（表记录 + 三个建表 tag + 每列一条 column 记录 + 库的 `next_table_id`），只有「这个名字已经有记录」时才回退到整份 `rewrite_catalog` 的 sweep。名字解析是点查（`read_cpp_table` 扫 `catalog|tbl|{db_id}|{name}|`），不再为取一张表把全目录每张表的每一列都解码；`DROP DATABASE` 取「这个库有哪些表」也只扫这个库的前缀。提交期没有任何持久化动作：DDL 的 catalog 记录带 `disableWAL` 写进 RocksDB memtable，durability 由 checkpoint 负责（见 [`CATALOG_BACKEND_FAQ.md`](CATALOG_BACKEND_FAQ.md) §3）。一次 DDL 既不做 memtable flush，也不 fsync，所以 10,000 条 DDL 不会堆出成千上万个小 L0 文件，每次提交的成本与库数无关。

**Gate A 由 `sqllogictest-rs` 执行**（参考 runner 用的同一个 crate，v0.29.1）：`cli conformance test/sql` 每个文件开一个新引擎，解析、record 语义和校验全交给这个 crate，任一文件失败就退出非零。当前 208 个文件里 199 个通过、9 个跳过、0 个失败。

跳过的 9 个文件需要重写版没有实现的导入格式，名单写在 `cli/src/main.rs` 的 `UNIMPLEMENTED_FORMATS` 常量里（每次运行都会打印），其余文件失败就会让门禁变红：

| 格式 | 文件 | 现状 |
| --- | --- | --- |
| `FORMAT PARQUET` | `dml/import/test_import_gen_parquet{,_embedding,_multivector,_sparse,_tensor,_tensor_array,_varchar}.slt` | 没有 parquet/arrow 读取，落到文本路径后报 `stream did not contain valid UTF-8` |
| `bvecs` | `dml/import/test_bvecs.slt` | 二进制向量文件未实现 |
| `csr` | `dml/import/test_csr.slt` | 稀疏矩阵文件未实现 |

三处与参考不一致的地方已经按参考口径对齐，不再需要跳过：

- **CSV 导入的整数溢出**：参考的 `ColumnVector::AppendByStringView` 用 `std::from_chars` 把单元格读进列自己的宽度，越界时目标保持初始 0 且 `ptr == end`（不报错），所以 `TINYINT` 列里的 `5000` 存成 0；重写版按同样口径处理（`zero_out_of_range_int`），`dql/aggregate/test_simple_agg_big` 通过。
- **浮点单元格的文本**：embedding/tensor 元素按 C++ `std::format("{}")` 渲染——指数小于 -4 或大于等于有效位数时用科学计数法（`fmt_f32_cpp` / `fmt_f64_cpp`），`dml/import/test_fvecs` 通过。
- **谓词扫描的墓碑**：`direct_projection` 的等值过滤分支自己读块，不经过 `try_rows` 的删除过滤；现在它同样丢掉删除日志里的行，`dml/compact/test_compact_big` 通过。恢复快照时列文件要跟着新表的列 id 改名：`DROP COLUMN` 之后幸存列保留原 id（`1.col`、`2.col`），而 `create_table_typed` 把重建表的列从 0 编号，读侧按 `meta.ids` 找不到 `0.col` 就整块解码失败、表读成空（`substitute_column_id`）。

### 2.1 事务冲突检查 / 回滚

**本节没有待办项。** 下面是这一层现在的口径。

参考的冲突检查是**按事务类型成对判定**（`NewTxn::CheckConflictTxnStore` 的二十多个重载）。端口移植了同一套矩阵：
`txn/src/conflict.rs` 的 `TxnOp`（= 参考 `BaseTxnStore::type_` 加上规则要读的字段）与 `conflict_between`，
每个写语句在提交前用 `Txn::set_op` 声明自己的类型（INSERT/INSERT…SELECT/Thrift Insert → `Append`，COPY 与
`import_literals` → `Import`，UPDATE → `Update{row_ids, segments}`，DELETE → `Delete{row_ids}`，COMPACT → `Compact`，
DUMP → `DumpMemIndex`，OPTIMIZE → `OptimizeIndex{entries}`（一次 optimize 一个事务，逐索引段记录），DDL 各自一种，
快照 → `Snapshot`，checkpoint / cleanup → 由 `TxnManager` 自己登记）。

判定是**有方向的**，与参考相同：只有后提交的一方跑检查、只有它自己的重载生效（所以 `Append` 看得到 `DropColumn`
（4005），`DropColumn` 自己却没有重载，反过来的顺序不冲突）。

候选集 `TxnManager::candidates` 是参考的 `check_txn_map_`，可见性判据也按参考的两个时间戳：每条事务在 `begin`
时冻结管理器的 `(last_kv_commit_ts, last_commit_ts)` 对（`NewTxnManager::BeginTxnShared` 把它交给 `NewTxn`），
候选条目带自己的 `kv_commit_ts`（未提交 = `UNCOMMIT_TS`），`GetCheckCandidateTxns` 的判定逐条照搬：

- `other_kv_commit_ts < begin_ts` → 已在本事务快照之前可见，跳过；
- 相等时用冻结的那对消歧：`< last_kv` 跳过；`== last_kv` 且 `other_commit_ts <= last_commit` 跳过；否则冲突；
- `other_begin_ts > this_commit_ts` → 后面的候选只会更晚，跳出。

端口把 KV 批放在提交里落（`apply_split`），所以 `kv_commit_ts = commit_ts + 1`——参考在 `CommitKVInstance` 里读
`current_ts_ + 1`，端口读同一个位置的唯一可能值。候选按 `kv_commit_ts < 最老活跃 begin_ts` 裁剪
（`CleanupTxnBottomNolock`），键日志（未分类语句的兜底）同一把尺子。

**行级判定没有 `del:` 键**：DELETE/UPDATE 把行 id 放进 `TxnOp`，按交集比较（参考的 `row_ids_`）；块的 `version`
是唯一的墓碑记录，KV 里没有任何 `del:` 记录（`blocks::tests::the_blocks_are_the_row_set_not_just_the_values`
断言 `row:` 与 `del:` 前缀都为空）。键集比较只留给没有分类的语句。

**读按语句的 `begin_ts`**：`execute_stmt` 进入时把本线程的读时间戳设成 `current_ts + 1`（`NewTxn::BeginTS`），
块的解码按 `version` 的 `create_ts` 与 `deleted` 裁剪（`created_at` / `slot_visible`），缓冲区里的行按它所属的
append 记录的时间戳裁剪（`AppendView::covers`），删除日志按删除的 commit ts 裁剪（`tombstone_set_at`），
`row_count` 用同一把尺子；行缓存（解码结果与行集缓存）在 pin 住时绕过。语句之外——checkpoint、回放、后台任务、
直接调用读接口——仍是"最新"。

**OPTIMIZE 的事务范围**：`OptimizationProcessor::NewNotifyOptimize` 一次触发开**一个**事务（`OptimizeAllIndexes`
把每个索引段的结果塞进同一个 `OptimizeIndexTxnStore::entries_`），即使什么都没合也照常提交（空 store 也花 commit ts；
WAL 帧里是逐条目的 `DumpIndexV2`，没有条目时就是一条无命令的帧）。端口相同：`optimize_cpp_index_chunks` 先取号开事务，再扫索引，`TxnOp::OptimizeIndex`
逐（表, 索引, 段）记录它真正重写过的条目，成功提交、失败回滚、冲突记为日志后放弃；条目字段决定每条规则（表级对
Append/Update/快照，段级对 Compact，索引级对 DumpMemIndex/DropIndex，跨表时对任何 DropIndex 都冲突）。

条目同时是 WAL 与弃用的载体：`OptimizeEntry` 带 `new_chunk_infos_`（合并后 chunk 的 `chunk_id`/`base_row_id`/`row_cnt`）
与 `deprecate_chunks_`，提交时按 `OptimizeIndexTxnStore::ToWalEntry` 每条写一条 `WalCmdDumpIndexV2`（参考的
`dump_cause_` 不进帧）。提交成功后按参考的两步弃用：`RemoveChunkIDs(deprecate_ids)` 把旧 chunk 从段的 chunk 列表（`idx_chunk|…`）里
去掉，并按 `PrepareCommitDumpIndex` 给每个 id 写一条 `drop|idx_chunk|…` 标记（值 = 提交 ts）；文件不动，读路径按标记过滤。
字节由之后一次 `cleanup_cpp_index_chunks` → `reclaim_deprecated_cpp_chunks` 删除，删完才清标记与记录。
测试：`tests::optimize_logs_one_dump_command_per_entry`。

已经对齐的部分：begin_ts 取 `current_ts + 1` 且不推进时钟、冲突发生在写 WAL 之前、冲突不写 WAL、
回滚不回退行号/段号、冲突也推进可见时钟、各类规则的表级/行级/段级粒度（见 `conflict.rs` 的单测）、
`retry_query` 的两档错误（4002 重试 / 4005 直返）、`QueryContext::QueryStatement` 的整句重试、
IMPORT 的块与 `create` 边界只在提交之后落（失败不留幽灵行）、`APPLIED_TS_KEY` 最后写。

测试：`txn` 的 `conflict::tests::*`（矩阵逐条，含 `an_optimize_pass_conflicts_per_entry`）、
`tests::a_read_at_an_older_ts_hides_later_commits`（读按 begin_ts）、
`tests::a_snapshot_that_saw_a_commit_does_not_conflict_with_it`（KV 提交时间戳的三段判据）、`conflict_check_ut::conflict_check_delete`（C++ `conflict_check_ut.cpp`）、
`request_ut::conflict_*`（C++ `conflict_request_ut.cpp` 的十个用例）。
事务这一层的 C++ 用例到端口映射见 [`TEST_AUDIT.md`](TEST_AUDIT.md)：`storage/txn/*` 15 个、
`new_catalog/new_txn_mgr_ut` 2 个（另 2 个 `DISABLED_SLOW_` 在端口是 `#[ignore]`）、
`cleanup_rollback_txn` 1 个，全部有对应用例。

**其余接线项当前为空。** 多 fragment/tasklet 与交换队列、`BottomExecutor` 与 `MemIndexAppender`、相关子查询、
压缩/`IMPORT` 的 WAL 帧、DDL 的 typed 命令、`COMPACT` 的候选集、`IndexScanBuilder`、`ResultCacheGetter`、
`MATCH TEXT` 走倒排、`fast_rough_filter`、`compact_interval`、分析器家族（含 `STEM_LANG_*` 全套词干）、
稀疏检索选项、快照，都按参考落地，逐条记录在 [`CLASS_MAP.md`](CLASS_MAP.md)（类/方法级）
与 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §6（门禁级）。

## 3 不可逐字节比对（参考侧不确定）

| 项 | 原因 | 门禁处理 |
|---|---|---|
| HNSW 图区 | `thread_local std::mt19937`，节点层级取决于线程调度；实测同一输入五次跑出五种图 | header/vectors/labels 字节比，图与查询答案按内容比（`FORMAT_EQUIVALENCE_TESTS.md` §6.4） |
| IVF 质心 / 训练 | `std::random_device` 采样，质心个数也不固定 | chunk 按内容比 |
| PLAID 的 chunk / `plaid_centroids.bin` | k-means 初始化与量化器训练都用 `random_device`，同一输入两次跑出的 centroid 与 bucket 权重都不同 | 按内容比：文件集、chunk 头部的文档数、侧文件的头部（dims/nbits/centroid 数/has_quantizer）与按布局算出的长度（`test_the_plaid_chunk_matches_by_content`） |
| `catalog/` 的 RocksDB 文件字节 | 带 ts / LSN / session id，参考自己两次跑也不同 | 键集与同键的值逐字节比（`test_the_catalog_key_set_and_values_match_exactly`） |
| NULL 行定长 slot、`VARCHAR` 内联 padding | 参考不写这些字节（未初始化内存，实测随前一次分配变化） | 新进程、该列首次写时逐字节相同；门禁按此安排 |
| 后台 dump 的 chunk 切分 | 配额驱动的队列 + 后台线程，切分是调度结果 | 关闭配额触发，只保留显式 `DumpIndex` 与行容量触发 |

## 4 覆盖不到的输入空间

细节（用例、规模、原因）在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5.2，这里只列名字。

**当前为空。** 逐个类型/规模的比对位置见
[`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5.1（逐文件的比对矩阵）。

## 5 已登记的行为差异（性能/口径，不是待办）

| 项 | 说明 | 出处 |
|---|---|---|
| 相关子查询的作用域边界 | 只支持引用**外层关系（FROM 侧）**的相关子查询：引用 JOIN 右侧表、引用派生表（parser 不保留派生表别名，标准 SQL 也要求限定）、或写在 `SEARCH` 的 `WHERE` filter 里时返回错误；不会静默按内层同名列求值（`substitute_outer_row` 只剥掉内层表自己的限定符） | `dependent.rs`、`substitute_outer_row` |
| `CREATE TABLE … AS SELECT` | 不实现（参考也没有）：`CREATE TABLE t AS SELECT …` 返回参考的 `Status::NoColumnDefined`（`Try to define Table t without any column.`，3048）；带列定义的 `CREATE TABLE t (a INT) AS SELECT …` 在参考是解析错误（`unexpected AS, expecting end of file`），端口同样拒绝 | `select_ddl_ut::create_table_as_select_is_rejected_like_the_reference` |
| `CREATE VIEW` / `DROP VIEW` | 不实现（参考也没有实现）：`PhysicalCreateView::Execute` 是空壳、DDL 分发没有这个分支，两侧都死在 `UnrecoverableException("Unknown DDL type")`（`query_context_impl.cpp:467`/`:489`），视图不会被登记，`FROM v` 随后报表不存在 | `select_ddl_ut::views_are_rejected_like_the_reference` |
| `INSERT … SELECT` | 只支持 select list 全为常量：参考的 `PhysicalInsert` 把投影表达式交给 `ExpressionState::CreateState`，那里没有 `Column` 分支，任何列引用（含 `SELECT *`）都会死在该处（`expression_state_impl.cpp:68`）。端口报同一条 `Unknown expression type: Column. expression name: {col}`，常量情形与参考一样按源行数插入 | `select_ddl_ut::insert_select_is_refused_when_the_select_list_reads_a_column` |
| `CREATE SNAPSHOT … ON TABLE <db>.<tbl>` | 参考的文法是 `IDENTIFIER`，带库名的写法是解析错误（`unexpected '.', expecting end of file`）；端口接受限定名（HTTP 层就按这个拼），无库名时用会话库 | `parser` 的 `CREATE SNAPSHOT` 分支（`parser.rs:362`） |

### 5.1 读路径比参考慢（已收窄，未追平）

逐阶段的耗时分布与架构对比见 [`SELECT_CHAIN.md`](SELECT_CHAIN.md)（同一台机、同一份数据、perf 采样）。这里只放结论：

| 查询（40,000 行，落块后，端到端 wall） | 参考 | 端口 |
|---|---|---|
| `SELECT id` | 2.1–2.4 ms | 5.3–5.5 ms |
| `SELECT id, name` | 16.2–16.7 ms | 25.1–26.6 ms |
| `WHERE id = 39999` | 1.3–2.8 ms | 2.1 ms |

SDK driver（20,000 行，未 checkpoint，8 次最好值）：参考 insert 0.293 s / select 9.45 ms / 点查 1.31 ms；端口 0.300 s / 11.6 ms / 3.3 ms。`check_benchmark_result.py` 比三阶段**均值**，0.30 s 的 insert 把两个读阶段摊薄成 ~+3%。

已改（`a5596c690`、`ac56246bf`、`7cb05cb00`、`46ded495f`、`859c3443e`、`2191fa03e`）：

1. **结果是类型化的，不再经文本**（`2191fa03e`）：`QueryResult::Rows` 从 `Vec<Vec<String>>` 改成 `Vec<Vec<Value>>`，协议层直接按 `Value` 打包，文本只在 CLI / `.slt` / HTTP / postgres 出口渲染。`SELECT id` 服务端 4.79 → 3.77 ms。
2. **纯列投影不再走算子链**（`direct_projection`）：`Projection(Scan)` / `Projection(Filter(Scan))` 直接出行，列被裁剪时按 `pos` 把行压回扫描列顺序。
3. **只解码用得到的列**：投影列 ∪ 谓词列（`predicate_columns`）交给块读，行宽不变、其余为 NULL。
4. **标量 cell 直接出 `Value`**（`decode_rows_of_values`）：整数/浮点/布尔从 slot 构造；文本型 `parse_owned_value` 把文本 move 进 `Str`。
5. **缓冲区不再绕文本**（`buffered_rows_values`），且只克隆投影要的 cell。
6. **列体每列切分一次**、`EMBEDDING` 形状每列解析一次：块读从 O(行数 × 块字节) 降下来（40,000 行 179 ms → 30 ms）。
7. **等值谓词在读出时执行**（`read_blocks_filtered_values`）：`col = 常量` 在行缓冲上求值，被拒的行不再各自分配。

块级 prefilter 与参考一致：`blk|…|fast_rough_filter` / `seg|…|fast_rough_filter` 只在段**封存**时写（import、compact、段写满 8,388,608 行），扫描读这些记录剪枝；未封存段两边都没有统计，端口不另做进程内缓存。

仍然慢的原因（详见 `SELECT_CHAIN.md` §6）：

1. **`Value` 行缓冲 vs 类型化列缓冲**：参考的扫描产出 `ColumnVector`（连续 `i32` + null 位图），协议层整块 `memcpy`；端口是 `Vec<Vec<Value>>`，每 cell 一个 24 字节枚举、每行一次分配。`SELECT id` 的 3.77 ms 里解码 2.1 ms，perf 里约一半在 malloc/free。
2. **文本型列必须解析**：varchar/json/date 只有文本形态；`SELECT id, name` 的解码 7.9 ms 里约 5.7 ms 是 varchar 这一列。
3. 这两条要根治得把结果与扫描都换成类型化列缓冲、表达式按列求值 —— 即把 `Value` 模型换成 `DataBlock` 模型（见 `chunk.rs` 的设计说明），属于另一层改造。
