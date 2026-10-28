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

| 项 | 现状 | 影响 | 依据 |
|---|---|---|---|
| ~~行以列式块为存储本体（方案 C）~~ **已完成** | 提交零 block 文件 I/O：行只进 `TableEntry::pending_rows`（`BufferObject` 的角色），块文件只由 checkpoint（`flush_cpp_blocks` → `write_cpp_blocks`）写；`row:`/`del:` 停写（只有块格式表达不了的列类型回落 KV）。读全部把 buffer 盖在块上：整表/剪枝/列投影/点取/含墓碑/计数/`row_is_present`。回放把 `WalCmd::Append`/`Delete` 交回 buffer（已落盘的 id 跳过），IMPORT 先写块再写 WAL 帧 | `persistence/` 不再有 `row:`/`del:` 子树（只剩侧车文件） | executor 698 passed / 29 ignored；catalog 54、storage 463、txn 21 全绿 |
| ~~`Cleanup` 之后 Rust 侧丢了 HNSW 的合并段 chunk（门禁 30）~~ **已修** | `prune_dead_segment_index_dirs` 曾用 `segment_ids()`（分配器，重开目录后为空）当"活段"，把每个 `idx_*/seg_4` 都删掉；改用 `visible_block_ids()`（块索引，跨重启有效） | 门禁 30：8 passed / 1 skipped | `cpp_blocks.rs::prune_dead_segment_index_dirs` |
| ~~门禁 40：SDK 单列读多值表拿不到行~~ **已修** | ①列投影在"本进程没写过镜像"（重开目录）时因缺快照直接放弃，现在块文件就是行集、快照只做并发守卫；②多值/张量表的 WAL 帧用本 build 的 `DataBlock` 还原不了行，checkpoint 没跑过就会丢——这类表额外保留一份文本行（`needs_text_rows`），块仍是权威 | 门禁 40：3 passed | `lib.rs::try_cpp_projection`、`needs_text_rows`；回归用例 `tests::a_multivector_table_round_trips_through_its_blocks` |
| ~~并发插入偶发丢行（`tests::concurrent_inserts_and_vector_searches_do_not_deadlock`）~~ **已修** | 根因是**分配与提交的次序**，不是 checkpoint 的写法：端口在执行期就发号（`reserve_rows`），参考在提交期发号且按 commit_ts 排序（`GetWriteCommitTS` 登记 → `SubmitForAllocation` 只从最小的 ts 放行 → `TxnAllocator::Process` → `PrepareAppend`）。执行期的号序和提交序不一致时，buffer 里会出现"高号已到、低号未到"，checkpoint 把块首当窗口起点就写歪（打点：`want=4 got=4 missing=[4,5] extra=2`）。现在：提交按 commit_ts 串行（`TxnManager::begin_write` / `commit_at_deferred_with`，对应 `wait_conflict_ck_`），行号在冲突检查之后、写 WAL 之前分配，`note_append` 当场带上 commit ts；checkpoint 的上界是 `current_ts`（已完成的 commit_ts），段的窗口按 `append_totals`+`append_ts` 裁到 `ts <= 上界`，块内 id 必须是窗口的稠密前缀（不再是"跳过带空洞的块"） | 全量 executor 698 passed / 29 ignored、txn 21、catalog 54、storage 463；字节门禁 00–60 全绿（新增 `tests::concurrent_appends_survive_a_checkpoint_and_reopen`、`cpp_blocks::tests::a_checkpoint_writes_only_the_committed_prefix`） | `infinity-txn::TxnManager::begin_write`、`lib.rs::committed_rows`、`cpp_blocks.rs::write_cpp_blocks_mode`；参考 `new_txn_manager_impl.cpp:207-220`、`840-881`、`new_txn_impl.cpp:1763-1764` |

### 2.1 事务冲突检查 / 回滚

参考的冲突检查是**按事务类型成对判定**（`NewTxn::CheckConflictTxnStore` 的二十多个重载），端口只有"KV 键相等 / 键前缀"两条规则；回滚一侧端口没有 RocksDB 事务可回滚，但也没有参考那套逐类型的清理。

| 项 | 参考口径 | 端口现状 | 影响 | 依据 |
|---|---|---|---|---|
| DML 不进冲突写集 | Append/Import 与 CreateIndex / DropIndex / AddColumn / DropColumn / DropTable / RenameTable 是**表级**冲突（后两类 `retry=false`） | INSERT / INSERT…SELECT / UPDATE / IMPORT 的行走 `typed_commands`，`Txn.writes` 为空，`conflict` 没有可比键 | 并发 DDL 与写入之间没有冲突检查；单进程 SQL 路径靠 `batch_gate` + 提交串行掩盖，多会话会读到半改的布局 | `new_txn_impl.cpp:3347-3424`、`:3426-3495`；`infinity-txn/src/lib.rs:518-560`；`infinity-executor/src/lib.rs:9748`、`:10085`、`:17089` |
| 无类型化规则矩阵 | Update-vs-Update **表级**、Update-vs-DumpMemIndex **段级**、Compact 与 Delete/Update/Create·DropIndex/Add·DropColumn 冲突、Dump-vs-Dump 直接 `UnrecoverableError` | 只有键相等/前缀：Update 只有行级；Compact 事务只有 typed 命令，**与谁都冲突不了**；两次并发的 dump 互不检测 | 并发 UPDATE 不同行在参考报 4002、端口都成功（结果不坏，口径不同）；COMPACT 与写入、dump 与 update 的互斥缺失 | `new_txn_impl.cpp:4313-4341`、`:3666-3760`、`:4095-4098`；`infinity-txn/src/lib.rs:518-560` |
| 无 4005、无整句重试 | `retry_query=false` 的冲突返回 4005；QueryContext 对 4002 **无限重跑整条语句** | 只有 4002；`Engine::execute` 每条语句跑一遍 | 冲突时客户端拿到错误而参考会自动重跑；"不可重试"的冲突类型（对象被 drop/rename、列被改）无法表达 | `query_context_impl.cpp:113-119`、`status.cppm:156-159`；`infinity-common/src/error.rs:31`、`:217` |
| 缺 `kv_commit_ts` 与候选窗口 | 冲突可见性按 **KV 真正提交的时刻**判，边界相等用 `last_kv_commit_ts` / `last_commit_ts` 两个冻结值消歧；候选集来自 `check_txn_map_`，按最老 begin_ts 裁剪 | 按"键最后一次写入的 ts > begin_ts"判；`write_ts` 永不裁剪 | 对端口自己的"提交即可见"模型等价，但边界语义不同；`write_ts` 随写入键数无界增长 | `new_txn_manager_impl.cpp:776-825`、`:669-694`；`infinity-txn/src/lib.rs:290`、`:380-385` |
| 读没有语句级快照 | 每条语句一个 `begin_ts` + RocksDB 快照，语句内看不到之后的提交 | 普通 SELECT 直接读最新已发布状态（读路径没有 ts 参数） | 一条 SELECT 执行期间别的语句提交的行会出现在结果里；跨语句不受影响（端口没有跨语句事务） | `new_txn_manager_impl.cpp:113`、`kv_store_impl.cpp:429-438`；`infinity-executor/src/lib.rs:14321-14332` |
| IMPORT/COPY 提交失败不回撤 | `PostRollback` 回滚 KV 事务、删 import 的临时/段目录、`ClearData` | 行在 `commit_at` **之前**就 `publish_rows`/`publish_encoded_rows` 进 buffer，并写 `appends:`/`append_ts:` 侧车；失败后都不撤 | 失败语句留下"幽灵行"：进程内 `row_count` 仍计入，但 checkpoint 按已提交 ts 裁剪不会写它们；只有 WAL/落盘失败能触发 | `new_txn_impl.cpp:4688-4745`；`infinity-executor/src/lib.rs:17102`、`:17489`、`cpp_blocks.rs:3366` |
| 提交批次不是原子的 | KV 是 RocksDB 事务，行批与目录批同生共死 | `apply_split` 先写行的批（里面带着 `APPLIED_TS_KEY`）、再写目录批；目录批失败/崩溃后 applied 标记已前进 | 崩溃可能丢一批目录记录（DDL），而重放按 applied 标记跳过它们 | `new_txn_impl.cpp:473`；`infinity-txn/src/lib.rs:782-786`、`:1232-1261` |

已经对齐的部分：begin_ts 取 `current_ts + 1` 且不推进时钟、冲突发生在写 WAL 之前、冲突不写 WAL、
回滚不回退行号/段号、冲突也推进可见时钟、Delete-vs-Delete 与 Delete-vs-Update 是行级。

**其余接线项当前为空。** 原先记在这里的接线项——多 fragment/tasklet 与交换队列、`BottomExecutor` 与
`MemIndexAppender`、相关子查询、压缩/`IMPORT` 的 WAL 帧、DDL 的 typed 命令、`COMPACT` 的候选集、
`IndexScanBuilder`、`ResultCacheGetter`、`MATCH TEXT` 走倒排、`fast_rough_filter`、`compact_interval`、
分析器家族、稀疏检索选项、快照——已经按参考落地，逐条记录在 [`CLASS_MAP.md`](CLASS_MAP.md)（类/方法级）
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
| `standard-porter` | 用 Porter2（`rust_stemmers::English`），少数词干与参考的 `STEM_LANG_PORT` 不同；其余 17 种语言同族一致 | `CLASS_MAP.md`「有意的设计变更」 |

