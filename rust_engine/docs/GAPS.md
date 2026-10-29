# 现状缺口清单

> **子目标 S7 · 缺口收口**｜纲领见 [`PORTING.md`](PORTING.md) §5

本文件是**唯一**的缺口汇总：哪些地方与参考不同、为什么、影响是什么。分四类，另有已登记的行为差异（§5，不是待办）：

- **§1 有意不移植**：已经决定不做，不是待办；
- **§2 未实现 / 未接线**：出现时附证据与影响；
- **§3 不可逐字节比对**：参考自身不确定或依赖实现细节，门禁只能按内容比；
- **§4 覆盖不到的输入空间**：机制有了，门禁还没跑到那个规模/形态。

格式层面的逐文件细节在 [`FORMAT_EQUIVALENCE_TESTS.md`](FORMAT_EQUIVALENCE_TESTS.md) §5；逐类/逐用例的映射状态在
[`CLASS_MAP.md`](CLASS_MAP.md) §2（`mapped` / `partial` / `excluded` 行）与 [`TEST_AUDIT.md`](TEST_AUDIT.md)（`excluded` 行，对应本文 §1 的不移植决定）。
**已接受的设计变更**（不是待办）登记在 [`CLASS_MAP.md`](CLASS_MAP.md)「有意的设计变更」，本文不重复。

---

## 1 有意不移植

| 项 | 决定 | 影响 | 依据 |
|---|---|---|---|
| 远程对象存储（`obj_key` 打包 + VirtualStore 上传、catalog 的 `meta/` 路径树同步） | 不移植；本地对象目录（`pm\|object\|` 记录 + 打包对象）仍按参考布局写 | 桶内布局与参考不同，跨引擎"同一桶互读"不成立 | `FORMAT_EQUIVALENCE_TESTS.md` §6.5 |
| DiskANN | 不移植 | 该索引类型不可用 | 计划书 §2.6/R14 |
| geo 类型（`Point`/`Line`/`LineSeg`/`Box`/`Circle`） | 不移植；`GeographyTryCastToVarlen` 随 cast 保留 | 这些列类型不可用 | `TEST_AUDIT.md` 的 `excluded` 行 |

## 2 未实现 / 未接线

**本节没有未接线项。** 这一层的现状：

- **行以列式块为存储本体**（方案 C）：提交不写 block 文件——行只进 `TableEntry::pending_rows`（`BufferObject` 的角色），块文件只由 checkpoint（`flush_cpp_blocks` → `write_cpp_blocks`）写；`row:` 停写（块格式表达不了的列类型另留一份文本行副本，见 `needs_text_rows`），`del:` 一律不写（块的 `version` 是唯一墓碑）。读全部把 buffer 盖在块上：整表 / 剪枝 / 列投影 / 点取 / 含墓碑 / 计数 / `row_is_present`；回放把 `WalCmd::Append`/`Delete` 交回 buffer（已落盘的 id 跳过），IMPORT 先写块再写 WAL 帧。`persistence/` 因此没有 `row:`/`del:` 子树，只剩侧车文件。
- **cleanup 的活段判据**是 `visible_block_ids()`（块索引，跨重启有效），不是 `segment_ids()`（分配器，重开目录后为空；用它会把每个 `idx_*/seg_{n}` 当死段删掉）。
- **行号的次序**：冲突检查之后、写 WAL 之前按 `commit_ts` 分配（`TxnManager::begin_write` / `commit_at_deferred_with`，对应参考的 `wait_conflict_ck_` / `SubmitForAllocation`），checkpoint 的上界是 `current_ts`（已完成的 commit_ts），块的窗口按 `append_totals` + `append_ts` 裁到 `ts <= 上界`，块内 id 必须是窗口的稠密前缀。
- **多值 / 张量表的文本行副本**：这类表的 WAL 帧用本 build 的 `DataBlock` 还原不了行，checkpoint 没跑过时靠 `needs_text_rows` 保留的文本行读出，块仍是权威。
- **列投影的并发守卫**：块文件就是行集，不需要「本进程写过镜像」的快照，快照只做并发守卫（`try_cpp_projection`）。

回归覆盖：`tests::concurrent_appends_survive_a_checkpoint_and_reopen`、`cpp_blocks::tests::a_checkpoint_writes_only_the_committed_prefix`、`tests::a_multivector_table_round_trips_through_its_blocks`、门禁 30 / 40。当前规模：executor 704 passed / 29 ignored、catalog 54、storage 463 passed / 5 ignored、txn 30，全绿。

**一处 `.slt` 仍未对齐（已复现，未修）**，Gate A 因此是 1556/1560：

- `ddl/drop/test_drop_column.slt` 4 条：`ALTER TABLE … DROP COLUMN` **之后**创建的 snapshot，`RESTORE TABLE SNAPSHOT` 回来行数翻倍（3 行变 6 行、再开一次变 9 行）并报 `block 0.0 holds 6 of the 6 rows committed at ts …`；快照里的块文件与 `version` 都是 3 行（已核对），所以问题在恢复后 WAL 回放把旧 append 再叠了一遍。最小复现：建三列表 → 插 3 行 → 建二级索引 → snapshot → `DROP COLUMN(c1)` → snapshot → `DROP TABLE` → `RESTORE` 第二个 snapshot → 再开一次进程读表。

### 2.1 事务冲突检查 / 回滚

**本节没有待办项。** 下面是这一层现在的口径。

参考的冲突检查是**按事务类型成对判定**（`NewTxn::CheckConflictTxnStore` 的二十多个重载）。端口移植了同一套矩阵：
`infinity-txn/src/conflict.rs` 的 `TxnOp`（= 参考 `BaseTxnStore::type_` 加上规则要读的字段）与 `conflict_between`，
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
是唯一的墓碑记录，KV 里没有任何 `del:` 记录（`cpp_blocks::tests::the_blocks_are_the_row_set_not_just_the_values`
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

测试：`infinity-txn` 的 `conflict::tests::*`（矩阵逐条，含 `an_optimize_pass_conflicts_per_entry`）、
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
| `CREATE SNAPSHOT … ON TABLE <db>.<tbl>` | 参考的文法是 `IDENTIFIER`，带库名的写法是解析错误（`unexpected '.', expecting end of file`）；端口接受限定名（HTTP 层就按这个拼），无库名时用会话库 | `infinity-parser` 的 `CREATE SNAPSHOT` 分支（`parser.rs:362`） |

